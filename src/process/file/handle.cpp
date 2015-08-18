#include <bunsan/process/file/handle.hpp>

#include <bunsan/process/file/error.hpp>
#include <bunsan/system_error.hpp>

#include <boost/assert.hpp>

#if defined(BOOST_POSIX_API)
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace bunsan {
namespace process {
namespace file {

handle::~handle() {
  if (m_close) close_no_except();
}

handle::handle(const implementation fd) : m_fd(fd) {}

handle::handle(const implementation fd, const bool close)
    : m_fd(fd), m_close(close) {}

handle::handle(handle &&o) {
  swap(o);
  o.reset();
}

handle &handle::operator=(handle &&o) {
  swap(o);
  o.reset();
  return *this;
}

void handle::reset() { handle().swap(*this); }

void handle::reset(const implementation fd) { handle(fd).swap(*this); }

void handle::close() {
  if (m_fd) {
    const int ret = ::close(*m_fd);
    m_fd = boost::none;
    if (ret) BOOST_THROW_EXCEPTION(bunsan::system_error("close"));
  }
}

void handle::close_no_except() noexcept {
  try {
    close();
  } catch (...) {
    // ignore
  }
}

handle handle::dup() const {
  if (!*this) BOOST_THROW_EXCEPTION(handle_is_closed_error());
  BOOST_ASSERT(m_fd);
  const implementation fd = ::dup(*m_fd);
  if (fd < 0)
    BOOST_THROW_EXCEPTION(system_error("dup") << system_error::handle(*m_fd));
  return handle(fd);
}

handle handle::dup2(const implementation new_fd) const {
  if (!*this) BOOST_THROW_EXCEPTION(handle_is_closed_error());
  BOOST_ASSERT(m_fd);
  const implementation fd = ::dup2(*m_fd, new_fd);
  if (fd < 0)
    BOOST_THROW_EXCEPTION(system_error("dup2")
                          << system_error::handle(*m_fd)
                          << system_error::new_handle(new_fd));
  return handle(fd);
}

namespace {
#if defined(BOOST_POSIX_API)
handle sys_open(const boost::filesystem::path &path, const int flags) {
  const int fd = ::open(path.string().c_str(), flags);
  if (fd < 0)
    BOOST_THROW_EXCEPTION(system_error("open") << system_error::path(path)
                                               << system_error::flags(flags));
  return file::handle(fd);
}

handle sys_open(const boost::filesystem::path &path, const int flags,
                const mode_t mode) {
  const int fd = ::open(path.string().c_str(), flags, mode);
  if (fd < 0)
    BOOST_THROW_EXCEPTION(system_error("open") << system_error::path(path)
                                               << system_error::flags(flags)
                                               << system_error::mode(mode));
  return file::handle(fd);
}
#elif defined(BOOST_WINDOWS_API)
template <typename... Args>
handle sys_open(const boost::filesystem::path &path, Args &&... args) {
  // TODO
}
#endif
}  // namespace

handle handle::open_null() {
#if defined(BOOST_POSIX_API)
  return open_read_write("/dev/null");
#elif defined(BOOST_WINDOWS_API)
  return sys_open("NUL", GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
#else
#error Unknown platform
#endif
}

handle handle::open_read(const boost::filesystem::path &path) {
#if defined(BOOST_POSIX_API)
  return sys_open(path, O_RDONLY);
#elif defined(BOOST_WINDOWS_API)
  // TODO
#else
#error Unknown platform
#endif
}

handle handle::open_write(const boost::filesystem::path &path) {
#if defined(BOOST_POSIX_API)
  return sys_open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
#elif defined(BOOST_WINDOWS_API)
  // TODO
#else
#error Unknown platform
#endif
}

handle handle::open_read_write(const boost::filesystem::path &path) {
#if defined(BOOST_POSIX_API)
  return sys_open(path, O_RDWR | O_CREAT, 0666);
#elif defined(BOOST_WINDOWS_API)
  // TODO
#else
#error Unknown platform
#endif
}

handle handle::open_append(const boost::filesystem::path &path) {
#if defined(BOOST_POSIX_API)
  return sys_open(path, O_WRONLY | O_APPEND | O_CREAT);
#elif defined(BOOST_WINDOWS_API)
  // TODO
#else
#error Unknown platform
#endif
}

handle handle::std_input() {
#if defined(BOOST_POSIX_API)
  return handle(STDIN_FILENO, false);
#elif defined(BOOST_WINDOWS_API)
  return handle(::GetStdHandle(STD_INPUT_HANDLE), false);
#else
#error Unknown platform
#endif
}
handle handle::std_output() {
#if defined(BOOST_POSIX_API)
  return handle(STDOUT_FILENO, false);
#elif defined(BOOST_WINDOWS_API)
  return handle(::GetStdHandle(STD_OUTPUT_HANDLE), false);
#else
#error Unknown platform
#endif
}
handle handle::std_error() {
#if defined(BOOST_POSIX_API)
  return handle(STDERR_FILENO, false);
#elif defined(BOOST_WINDOWS_API)
  return handle(::GetStdHandle(STD_ERROR_HANDLE), false);
#else
#error Unknown platform
#endif
}

}  // namespace file
}  // namespace process
}  // namespace bunsan
