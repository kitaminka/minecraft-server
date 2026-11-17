#include "application.hpp"
#include "io/signal_handler.hpp"
#include "network/tcp_server.hpp"

Application::Application() : epoll(), signal_handler(epoll), server(epoll, 25565) {}

void Application::start() {
  server.start();
  epoll.start_loop();
}
