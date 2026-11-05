#include <iostream>
#include "network/server.hpp"

int main() {
  Server s(25565);

  s.start();
}
