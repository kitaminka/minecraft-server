#include <unistd.h>

#include "network/client.hpp"
#include "io/unique_fd.hpp"

Client::Client(int fd, sockaddr_in address) : ufd(UniqueFd(fd)), address(address) {}
