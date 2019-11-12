project(StreamBase)
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (server "server.cpp" "server.h" "common.cpp" "common.h")
add_executable (exampleClient "exampleClient.cpp" "CustomClass.cpp" "CustomClass.h" "client.cpp" "client.h" "common.cpp" "common.h")
add_executable (exampleAsyncClient "exampleAsyncClient.cpp" "CustomClass.cpp" "CustomClass.h" "client.cpp" "client.h" "common.cpp" "common.h")
