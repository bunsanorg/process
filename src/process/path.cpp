#include <bunsan/process/path.hpp>

#include <bunsan/process/detail/path.hpp>

namespace bunsan {
namespace process {

boost::filesystem::path find_executable_in_path(
    const boost::filesystem::path &executable) {
  return detail::find_executable_in_path(executable);
}

}  // namespace process
}  // namespace bunsan
