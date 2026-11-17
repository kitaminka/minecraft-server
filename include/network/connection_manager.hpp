#pragma once

class IConnectionManager {
  public:
    virtual void disconnected(int fd) = 0;
    virtual ~IConnectionManager() = default;
};
