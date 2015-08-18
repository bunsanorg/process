#pragma once

// first includes
#include <boost/config.hpp>
#include <boost/system/api_config.hpp>

#include <boost/optional.hpp>

#if defined(BOOST_WINDOWS_API)
#include <windows.h>
#endif

namespace bunsan {
namespace process {
namespace file {

class handle {
 public:
#if defined(BOOST_POSIX_API)
  using implementation = int;
#elif defined(BOOST_WINDOWS_API)
  using implementation = HANDLE;
#endif

  handle() = default;
  ~handle();

  explicit handle(implementation fd);
  handle(implementation fd, bool close);

  handle(const handle &) = delete;
  handle(handle &&);

  handle &operator=(const handle &) = delete;
  handle &operator=(handle &&);

  explicit operator bool() const { return static_cast<bool>(m_fd); }
  implementation operator*() const { return *m_fd; }

  void reset();
  void reset(implementation fd);
  void close();
  void close_no_except() noexcept;

  void swap(handle &o) noexcept {
    using std::swap;

    swap(m_fd, o.m_fd);
    swap(m_close, o.m_close);
  }

  handle dup() const;
  handle dup2(implementation new_fd) const;

 private:
  boost::optional<implementation> m_fd;
  bool m_close = true;
};

inline void swap(handle &a, handle &b) noexcept { a.swap(b); }

handle stdin_handle();
handle stdout_handle();
handle stderr_handle();

}  // namespace file
}  // namespace process
}  // namespace bunsan
