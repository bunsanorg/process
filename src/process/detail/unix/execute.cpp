#include <bunsan/process/detail/execute.hpp>

#include "error.hpp"
#include "executor.hpp"
#include "open.hpp"

#include <bunsan/process/file/handle.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/noncopyable.hpp>
#include <boost/variant/static_visitor.hpp>

#include <iostream>

#include <unistd.h>
#include <sys/wait.h>

namespace bunsan {
namespace process {
namespace detail {

class file_action_visitor : public boost::static_visitor<void>,
                            private boost::noncopyable {
 public:
  file_action_visitor(const standard_file self, const mode_t mode)
      : m_self(self), m_mode(mode) {}

  void operator()(inherit_type) { m_fd = standard(m_self); }

  void operator()(suppress_type) { m_fd = open("/dev/null", O_RDWR); }

  void operator()(const standard_file file) { m_fd = standard(file); }

  void operator()(const boost::filesystem::path &path) {
    m_fd = open(path, m_mode, 0666);
  }

  void dispatch() {
    const file::handle self_fd = standard(m_self);
    if (*self_fd != *m_fd) m_fd = m_fd.dup2(*self_fd);
  }

 private:
  static file::handle standard(const standard_file file) {
    switch (file) {
      case stdin_file:
        return file::stdin_handle();
      case stdout_file:
        return file::stdout_handle();
      case stderr_file:
        return file::stderr_handle();
      default:
        return file::handle();
    }
  }

 private:
  const standard_file m_self;
  const mode_t m_mode;
  file::handle m_fd;
};

int sync_execute(const context &ctx) {
  executor exec_(ctx.executable, ctx.arguments);

  file_action_visitor stdin_visitor(stdin_file, O_RDONLY);
  file_action_visitor stdout_visitor(stdout_file, O_WRONLY | O_CREAT | O_TRUNC);
  file_action_visitor stderr_visitor(stderr_file, O_WRONLY | O_CREAT | O_TRUNC);
  boost::apply_visitor(stdin_visitor, ctx.stdin_file);
  boost::apply_visitor(stdout_visitor, ctx.stdout_file);
  boost::apply_visitor(stderr_visitor, ctx.stderr_file);

  const pid_t pid = ::fork();
  if (pid < 0) {
    BOOST_THROW_EXCEPTION(system_error("fork"));
  } else if (pid > 0) {
    for (;;) {
      int stat_loc;
      const pid_t rpid = ::waitpid(pid, &stat_loc, 0);
      if (rpid != pid) {
        BOOST_ASSERT_MSG(rpid < 0, "WNOHANG was not set");
        if (errno != EINTR) {
          BOOST_THROW_EXCEPTION(system_error("waitpid")
                                << system_error::pid(pid));
        }
      } else {
        if (WIFEXITED(stat_loc)) {
          return WEXITSTATUS(stat_loc);
        } else {
          BOOST_ASSERT(WIFSIGNALED(stat_loc));
          return -WTERMSIG(stat_loc);
        }
      }
    }
  } else {
    BOOST_ASSERT(pid == 0);
    try {
      // clear parent's sigprocmask
      sigset_t sset;
      sigemptyset(&sset);
      sigprocmask(SIG_SETMASK, &sset, nullptr);

      boost::filesystem::current_path(ctx.current_path);

      stdin_visitor.dispatch();
      stdout_visitor.dispatch();
      stderr_visitor.dispatch();

      exec_.exec();
    } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
      std::abort();
    } catch (...) {
      std::cerr << "Unknown exception." << std::endl;
      std::abort();
    }
  }
}

}  // namespace detail
}  // namespace process
}  // namespace bunsan
