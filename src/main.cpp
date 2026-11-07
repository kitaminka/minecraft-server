#include <iostream>
#include "network/server.hpp"

int main() {
  Server s(25565);

  s.start();

  std::string command;
  while(std::cin >> command) {
    if (command == "stop") {
      s.stop();
      break;
    }
  }
}
