#include <unistd.h>
#include <iostream>

#include "network/unique_fd.hpp"

UniqueFd::UniqueFd() : fd(-1) {}

UniqueFd::UniqueFd(int fd) : fd(fd) {}

UniqueFd::~UniqueFd() {
  if (is_active()) {
    std::cout << "Closing fd: " << fd << std::endl;
    close(fd);
    fd = -1;
  }
}

int UniqueFd::get_fd() const {
  return fd;
}

bool UniqueFd::is_active() const {
  return fd != -1;
}

UniqueFd& UniqueFd::operator=(UniqueFd&& other) noexcept{
  if (this != &other) {
    if (fd != 1) {
      close(fd);
    }
    fd = other.fd;
    other.fd = -1;
  }
  return *this;
}

UniqueFd::UniqueFd(UniqueFd&& other) noexcept : fd(other.fd) {
  other.fd = -1;
}
