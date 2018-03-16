#include "restsdk_https_client.h"

#include <cpprest/http_client.h>
#include <pplx/pplxtasks.h>

#include <vector>

namespace
{
#ifdef _WIN32
	static void SetNativeHandleOptions(web::http::client::native_handle handle)
	{
//		ucout << __FUNCTION__ << U(" - setting SSL context") << std::endl;
		HINTERNET hRequest = static_cast<HINTERNET>(handle);
		if (hRequest)
		{
			auto result = WinHttpSetOption(
				handle,
				WINHTTP_OPTION_CLIENT_CERT_CONTEXT,
				WINHTTP_NO_CLIENT_CERT_CONTEXT,
				0);

			if (!result)
			{
				ucout << __FUNCTIONW__ << U(" - Error setting WinHttp to not send a client certificate, code [")
					<< GetLastError() << U("]") << std::endl;
			}
		}
	}
#else
	static bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx)
	{
		ucout << __FUNCTION__ << U(" - invoked") << std::endl;

		char subject_name[256];
		X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
		X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
		std::cout << "Verifying " << subject_name << "\n";

		return preverified;
	}

	static void SetOpenSSLContextOptions(boost::asio::ssl::context& ctx)
	{
		try
		{
//			ucout << __FUNCTION__ << U(" - setting SSL context") << std::endl;
			//set handshake options
			auto sslOptions = 
				boost::asio::ssl::context::no_sslv2 |
				boost::asio::ssl::context::no_sslv3  |
				boost::asio::ssl::context::single_dh_use |
				boost::asio::ssl::context::default_workarounds;
			ctx.set_options(sslOptions);
			//ctx.set_default_verify_paths();

			ctx.set_verify_mode(boost::asio::ssl::verify_peer | boost::asio::ssl::verify_fail_if_no_peer_cert);
			//ctx.set_verify_mode(boost::asio::ssl::verify_none);
			
			ctx.load_verify_file("ca.pem");
			ctx.set_verify_callback(verify_certificate);
		}
		catch (std::exception& ex)
		{
			ucout << __FUNCTION__ << " - exception : " << ex.what() << std::endl;
		}
	}
#endif
}

#ifndef _WIN32
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/x509.h>

struct OpenSSLLoader
{
	OpenSSLLoader()
	{
		OpenSSL_add_all_algorithms();
		ERR_load_crypto_strings();
	}

	~OpenSSLLoader()
	{
#ifndef OPENSSL_NO_ENGINE
		ENGINE_cleanup();
#endif
		do
		{
			CONF_modules_unload(1);
			OBJ_cleanup();
			EVP_cleanup();
			CRYPTO_cleanup_all_ex_data();
			ERR_remove_thread_state(NULL);
			RAND_cleanup();
			ERR_free_strings();
		} while (0);
	}
};
#endif

int main(int argc, char* argv[])
{
#ifdef _WIN32
#else
	OpenSSLLoader ssl{};
#endif

	// http config
	web::http::client::http_client_config config;

#ifdef _WIN32
	config.set_validate_certificates(false);
	config.set_nativehandle_options(SetNativeHandleOptions);
#else
	config.set_validate_certificates(true);
	config.set_ssl_context_callback(SetOpenSSLContextOptions);
#endif

	// http client
	web::http::uri uri(U("https://www.google.com"));
	web::http::client::http_client client(uri, config);

	// request
	web::http::http_request request(web::http::methods::GET);
	request.set_request_uri(U(""));

	auto process_response = [](web::http::http_response response)
	{
		// Display the status code that the server returned
		utility::ostringstream_t stream;
		stream << U("HTTP code: [") << response.status_code() << U("]") << std::endl;
		ucout << stream.str();

		stream.str(utility::string_t());
		stream << U("Content type: [") << response.headers().content_type() << U("]") << std::endl;
		stream << U("Content length: [") << response.headers().content_length() << U(" bytes ]") << std::endl;
		ucout << stream.str();

		auto bodyStream = response.body();
		Concurrency::streams::stringstreambuf sbuffer;
		auto& target = sbuffer.collection();

		bodyStream.read_to_end(sbuffer).get();

		stream.str(utility::string_t());
		stream << U("Response body: [") << target.length() << U(" symbols ]") << std::endl;
		ucout << stream.str();
	};

	// create 10 tasks requesting data
	std::vector<pplx::task<void>> tasks;
	tasks.resize(10);
	for (auto& t : tasks)
	{
		t = client.request(request)
			.then(process_response)
			.then([=](pplx::task<void> previous_task) mutable {
			if (previous_task._GetImpl()->_HasUserException()) {
				try {
					ucout << U("=> request is canceled ") << std::endl;
					auto holder = previous_task._GetImpl()->_GetExceptionHolder();
					holder->_RethrowUserException();
				}
				catch (std::exception& e) {
				}
			}
		});
	}

	auto joinTask = pplx::when_all(std::begin(tasks), std::end(tasks));

	ucout << U("waiting for tasks...");
	joinTask.wait();
	ucout << U("done") << std::endl;

	getchar();

	return 0;
}

