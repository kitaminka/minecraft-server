cmake_minimum_required(VERSION 3.23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

project(minecraft_server)

add_executable(minecraft_server)

find_package(Threads)
target_link_libraries(minecraft_server PRIVATE Threads::Threads)

target_compile_options(minecraft_server PRIVATE -Wall -Wextra -Wpedantic -Wold-style-cast)

target_include_directories(minecraft_server
  PRIVATE
    include
)

target_sources(minecraft_server
  PRIVATE
    src/main.cpp
    src/network/tcp_server.cpp
    src/network/tcp_connection.cpp
    src/io/unique_fd.cpp
    src/io/epoll.cpp
)
