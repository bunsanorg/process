#pragma once

#include <bunsan/process/error.hpp>
#include <bunsan/process/file/handle.hpp>

#include <bunsan/filesystem/error.hpp>

namespace bunsan {
namespace process {
namespace file {

struct error : virtual process::error {
  using handle = boost::error_info<struct handle, file::handle::implementation>;
  using new_handle =
      boost::error_info<struct new_handle, file::handle::implementation>;
};

struct system_error : bunsan::filesystem::system_error, virtual error {
  using bunsan::filesystem::system_error::system_error;
};

struct handle_error : virtual error {};
struct handle_is_closed_error : virtual error {};

}  // namespace file
}  // namespace process
}  // namespace bunsan
