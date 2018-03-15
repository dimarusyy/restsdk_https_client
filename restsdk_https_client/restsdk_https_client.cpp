#include "restsdk_https_client.h"

#include <cpprest/http_client.h>
#include <pplx/pplxtasks.h>

#include <vector>

namespace
{
#ifdef _WIN32
	void SetNativeHandleOptions(web::http::client::native_handle handle)
	{
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
#endif
}

int main(int argc, char* argv[])
{
	// http config
	web::http::client::http_client_config config;
	config.set_validate_certificates(false);
#ifdef _WIN32
	config.set_nativehandle_options(&SetNativeHandleOptions);
#else
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
	tasks.resize(50);
	for (auto& t : tasks)
	{
		t = client.request(request).then(process_response);
	}

	auto joinTask = pplx::when_all(std::begin(tasks), std::end(tasks));

	ucout << U("waiting for tasks...");
	joinTask.wait();
	ucout << U("done") << std::endl;

	getchar();

	return 0;
}

