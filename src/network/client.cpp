#include <unistd.h>

#include "network/client.hpp"
#include "network/unique_fd.hpp"

Client::Client(int fd, sockaddr_in address) : fd(UniqueFd(fd)), address(address) {}
