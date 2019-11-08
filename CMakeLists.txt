# CMakeList.txt : CMake project for StreamBase, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (client "client.cpp" "client.h")
add_executable (server "server.cpp" "server.h")


# TODO: Add tests and install targets if needed.
