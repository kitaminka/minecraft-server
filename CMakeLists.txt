cmake_minimum_required(VERSION 3.23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set (CMAKE_CXX_STANDARD 20)

project(minecraft_server)

add_executable(minecraft_server)

find_package(Threads)
find_package(OpenSSL)
target_link_libraries(minecraft_server PRIVATE Threads::Threads OpenSSL::Crypto)

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
    src/application.cpp
    src/log/logger.cpp
    src/io/signal_handler.cpp
    src/protocol/byte_reader.cpp
    src/protocol/byte_writer.cpp
    src/protocol/minecraft_client.cpp
)

find_program(CLANG_FORMAT clang-format)

if(CLANG_FORMAT)
  file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
  )

  add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i ${ALL_CXX_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Formatting source files with clang-format..."
  )

  add_custom_target(check-format
    COMMAND ${CLANG_FORMAT} --dry-run --Werror ${ALL_CXX_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Checking code formatting with clang-format..."
  )
else()
  message(WARNING "clang-format not found.")
endif()
