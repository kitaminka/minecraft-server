cmake_minimum_required(VERSION 3.23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(minecraft_server)

add_executable(minecraft_server)

target_include_directories(minecraft_server
  PRIVATE
    include
)

target_sources(minecraft_server
  PRIVATE
    src/main.cpp
    src/network/server.cpp
)
