#pragma once

#include <netinet/in.h>

class Client {
  public:
    int fd;
    sockaddr_in address;

    Client(int fd, sockaddr_in address);
    ~Client();
};
