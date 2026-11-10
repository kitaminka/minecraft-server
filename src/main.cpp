#include <signal.h>
#include <pthread.h>
#include <exception>
#include <iostream>
#include <thread>

#include "network/tcp_server.hpp"

void signal_watcher(const sigset_t* sigmask, TcpServer* s) {
  int sig;
  sigwait(sigmask, &sig);
  s->stop();
}

int main() {
  sigset_t sigmask;
  sigemptyset(&sigmask);
  sigaddset(&sigmask, SIGINT);
  sigaddset(&sigmask, SIGTERM);
  pthread_sigmask(SIG_BLOCK, &sigmask, nullptr);

  TcpServer s(25565);

  try {
    s.start();
  } catch(const std::exception& e) {
    std::cerr << e.what() << std::endl;
    s.stop();
    return 1;
  }

  auto signal_thread = std::thread(signal_watcher, &sigmask, &s);

  // std::string command;
  // while(std::cin >> command) {
  //   if (command == "stop") {
  //     s.stop();
  //     break;
  //   }
  // }
  if (signal_thread.joinable()) {
    signal_thread.join();
  }

  if (signal_thread.joinable()) {
    pthread_kill(signal_thread.native_handle(), SIGTERM);
    signal_thread.join();
  }

  return 0;
}
