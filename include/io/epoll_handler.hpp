#pragma once

class IEpollHandler{
  public:
    virtual ~IEpollHandler() = default;
    virtual void on_read() = 0;
};
