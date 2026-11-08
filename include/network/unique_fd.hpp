#pragma once

class UniqueFd {
  int fd{-1};

  public:
    UniqueFd();
    UniqueFd(int fd);

    UniqueFd(const UniqueFd&) = delete;
    UniqueFd& operator=(const UniqueFd&) = delete;

    UniqueFd(UniqueFd&& other) noexcept;
    UniqueFd& operator=(UniqueFd&& other) noexcept;

    ~UniqueFd();

    int get_fd() const;
    bool is_active() const;
};
