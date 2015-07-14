#pragma once

#include <bunsan/process/detail/error.hpp>

#include <bunsan/filesystem/error.hpp>

#include <sys/types.h>

namespace bunsan {
namespace process {
namespace detail {

struct system_error : bunsan::filesystem::system_error, virtual error {
  using bunsan::filesystem::system_error::system_error;

  using pid = boost::error_info<struct tag_pid, pid_t>;
  using fd = boost::error_info<struct tag_fd, int>;
  using new_fd = boost::error_info<struct tag_new_fd, int>;
  using flags = boost::error_info<struct tag_flags, int>;
  using mode = boost::error_info<struct tag_mode, mode_t>;
};

struct descriptor_error : virtual error {};
struct descriptor_is_closed_error : virtual error {};

}  // namespace detail
}  // namespace process
}  // namespace bunsan
