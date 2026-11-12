#include <csignal>
#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <iostream>

#include "io/signal_handler.hpp"
#include "io/epoll.hpp"
#include "io/unique_fd.hpp"

SignalHandler::SignalHandler(Epoll& epoll) : epoll(epoll) {
  sigset_t sigmask;
  sigemptyset(&sigmask);
  sigaddset(&sigmask, SIGINT);
  sigaddset(&sigmask, SIGTERM);

  pthread_sigmask(SIG_BLOCK, &sigmask, nullptr);

  ufd = UniqueFd(signalfd(-1, &sigmask, SFD_NONBLOCK | SFD_CLOEXEC));
  epoll.add(ufd.get_fd(), this);
}

void SignalHandler::on_read() {
  signalfd_siginfo info;
  read(ufd.get_fd(), &info, sizeof(info));
  std::cout << "Got signal: " << info.ssi_code << std::endl;

  if (info.ssi_signo == SIGINT || info.ssi_signo == SIGTERM)  {
    std::cout << "Stopping server..." << std::endl;
    epoll.stop_loop();
  }
}
