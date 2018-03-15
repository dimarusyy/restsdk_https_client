# CMakeList.txt : CMake project for restsdk_https_client, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Add source to this project's executable.
add_executable (restsdkhttpsclient "restsdk_https_client.cpp" "restsdk_https_client.h")

#extra libraries on win32
if(WIN32)
    target_link_libraries(restsdkhttpsclient bcrypt.lib)
	target_link_libraries(restsdkhttpsclient secur32.lib)
	target_link_libraries(restsdkhttpsclient crypt32.lib)
	target_link_libraries(restsdkhttpsclient winhttp.lib)
endif(WIN32)
