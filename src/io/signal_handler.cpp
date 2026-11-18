#include <csignal>
#include <sys/signalfd.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "io/signal_handler.hpp"
#include "io/epoll.hpp"
#include "io/unique_fd.hpp"
#include "log/logger.hpp"

SignalHandler::SignalHandler(Epoll& epoll) : epoll(epoll) {
  sigset_t sigmask;
  sigemptyset(&sigmask);
  sigaddset(&sigmask, SIGINT);
  sigaddset(&sigmask, SIGTERM);

  pthread_sigmask(SIG_BLOCK, &sigmask, nullptr);

  ufd = UniqueFd(signalfd(-1, &sigmask, SFD_NONBLOCK | SFD_CLOEXEC));
  logger::debug("Created signal handler file descriptor: {}", ufd.get_fd());
  epoll.add(ufd.get_fd(), this);
}

SignalHandler::~SignalHandler() {
  epoll.remove(ufd.get_fd());
}

void SignalHandler::on_read() {
  signalfd_siginfo info;
  read(ufd.get_fd(), &info, sizeof(info));
  logger::debug("Got signal: {}", strsignal(info.ssi_signo));

  if (info.ssi_signo == SIGINT || info.ssi_signo == SIGTERM)  {
    logger::info("Stopping server...");
    epoll.stop_loop();
  }
}
