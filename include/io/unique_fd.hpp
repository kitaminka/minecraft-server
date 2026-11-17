#pragma once

class UniqueFd {
  public:
    UniqueFd();
    explicit UniqueFd(int fd);

    UniqueFd(const UniqueFd&) = delete;
    UniqueFd& operator=(const UniqueFd&) = delete;

    UniqueFd(UniqueFd&& other) noexcept;
    UniqueFd& operator=(UniqueFd&& other) noexcept;

    ~UniqueFd();

    int get_fd() const;
    bool is_active() const;
    void reset();
  private:
    int fd{-1};
};
