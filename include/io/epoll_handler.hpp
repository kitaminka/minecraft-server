#pragma once

class IEpollHandler{
  public:
    virtual ~IEpollHandler() = default;
    virtual void on_read() = 0;
    virtual void on_write() {};
};
