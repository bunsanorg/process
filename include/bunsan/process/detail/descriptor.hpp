#pragma once

#include <boost/config.hpp>  // first include

#include <boost/optional.hpp>

#if defined(BOOST_WINDOWS_API)
#include <windows.h>
#endif

namespace bunsan {
namespace process {
namespace detail {

class descriptor {
 public:
#if defined(BOOST_POSIX_API)
  using handle = int;
#elif defined(BOOST_WINDOWS_API)
  using handle = HANDLE;
#endif

  descriptor() = default;
  ~descriptor();

  explicit descriptor(handle fd);
  descriptor(handle fd, bool close);

  descriptor(const descriptor &) = delete;
  descriptor(descriptor &&);

  descriptor &operator=(const descriptor &) = delete;
  descriptor &operator=(descriptor &&);

  explicit operator bool() const { return static_cast<bool>(m_fd); }
  handle operator*() const { return *m_fd; }

  void reset();
  void reset(const handle fd);
  void close();
  void close_no_except() noexcept;

  void swap(descriptor &o) noexcept {
    using std::swap;

    swap(m_fd, o.m_fd);
    swap(m_close, o.m_close);
  }

  descriptor dup() const;
  descriptor dup2(handle new_fd) const;

 private:
  boost::optional<handle> m_fd;
  bool m_close = true;
};

inline void swap(descriptor &a, descriptor &b) noexcept { a.swap(b); }

descriptor stdin_descriptor();
descriptor stdout_descriptor();
descriptor stderr_descriptor();

}  // namespace detail
}  // namespace process
}  // namespace bunsan
