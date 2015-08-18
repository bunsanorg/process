#include <bunsan/process/file/handle.hpp>

#include <bunsan/process/file/error.hpp>
#include <bunsan/system_error.hpp>

#include <boost/assert.hpp>

#include <unistd.h>

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

handle stdin_handle() { return handle(STDIN_FILENO, false); }

handle stdout_handle() { return handle(STDOUT_FILENO, false); }

handle stderr_handle() { return handle(STDERR_FILENO, false); }

}  // namespace file
}  // namespace process
}  // namespace bunsan
