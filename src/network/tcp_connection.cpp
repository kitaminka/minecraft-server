#include <unistd.h>

#include "network/tcp_connection.hpp"
#include "io/unique_fd.hpp"

TcpConnection::TcpConnection(int fd, sockaddr_in address) : ufd(UniqueFd(fd)), address(address) {}
