#dependencies

#cpprestsdk
if(MSVC)
	find_library(CPPREST_LIBRARY cpprest_2_10)
else()
	find_library(CPPREST_LIBRARY cpprest HINTS "${CPPREST_ROOT}/lib")
	set(CPPREST_INCLUDE_DIR "${CPPREST_ROOT}/include")
endif()
find_path(CPPREST_INCLUDE_DIR cpprest/version.h)
if(CPPREST_INCLUDE_DIR MATCHES "NOTFOUND" OR CPPREST_LIBRARY MATCHES "NOTFOUND")
	message(FATAL_ERROR "Could not find cpprestsdk")
else()
	include_directories(${CPPREST_INCLUDE_DIR})
	link_libraries(${CPPREST_LIBRARY})
	message(STATUS "Found cpprestsdk: [${CPPREST_INCLUDE_DIR}] [${CPPREST_LIBRARY}]")
endif()

#boost
find_package(Boost REQUIRED system filesystem date_time)
if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_libraries(${Boost_LIBRARIES})
	message(STATUS "Found boost v.${Boost_VERSION}: [${Boost_INCLUDE_DIRS}] [${Boost_LIBRARIES}]")
else()
	message(FATAL_ERROR "Could not find boost")
endif()

#openssl
find_package(OpenSSL REQUIRED)
if (OpenSSL_FOUND)
	include_directories(${OPENSSL_INCLUDE_DIR})
	link_libraries(${OPENSSL_LIBRARIES})
	message(STATUS "Found OpenSSL v.${OPENSSL_VERSION}: [${OPENSSL_INCLUDE_DIR}] [${OPENSSL_LIBRARIES}]")
else()
	message(FATAL_ERROR "Could not find openssl")
endif()

#zlib
find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
	include_directories(${ZLIB_INCLUDE_DIRS})
	link_libraries(${ZLIB_LIBRARIES})
	message(STATUS "Found zlib v.${ZLIB_VERSION_STRING}: ${ZLIB_LIBRARIES}")
else()
	message(FATAL_ERROR "Could not find zlib")
endif()
