#pragma once

class IConnectionManager {
  public:
    virtual void disconnect(int fd) = 0;
    virtual void enable_writing_flag(int fd) = 0;
    virtual void disable_writing_flag(int fd) = 0;
    virtual ~IConnectionManager() = default;
};
