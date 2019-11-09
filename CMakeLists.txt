# CMakeList.txt : CMake project for StreamBase, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_library(serialization serialization.cpp)

# Add source to this project's executable.
add_executable (client "client.cpp" "client.h")
add_executable (server "server.cpp" "server.h")

target_link_libraries(client serialization)
target_link_libraries(server serialization)