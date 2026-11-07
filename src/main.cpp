#include <exception>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include "network/server.hpp"

Server s(25565);

void signal_handler(int) {
  s.request_stop();
  close(STDIN_FILENO);
}

int main() {
  signal(SIGINT, signal_handler);

  try {
    s.start();
  } catch(const std::exception& e) {
    std::cerr << e.what() << std::endl;
    s.stop();
    return 1;
  }

  std::string command;
  while(std::cin >> command) {
    if (command == "stop") {
      s.stop();
      break;
    }
  }

  return 0;
}
