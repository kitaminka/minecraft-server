#include "application.hpp"
#include "io/signal_handler.hpp"
#include "network/tcp_server.hpp"

Application::Application() : epoll(),  server(epoll, 25565), signal_handler(epoll) {}

void Application::start() {
  server.start();
  epoll.start_loop();
}
