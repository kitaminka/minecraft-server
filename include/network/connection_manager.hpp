#pragma once

class IConnectionManager {
  public:
    virtual void remove_connection(int fd) = 0;
    virtual ~IConnectionManager() = default;
};
