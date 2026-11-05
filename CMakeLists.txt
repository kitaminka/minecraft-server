cmake_minimum_required(VERSION 3.23)

project(minecraft_server)

add_executable(minecraft_server)

target_sources(minecraft_server
  PRIVATE
    src/main.cpp
)
