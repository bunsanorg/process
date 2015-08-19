#include <bunsan/process/path.hpp>

#include <bunsan/process/error.hpp>

#include <windows.h>

#include <cstdlib>

namespace bunsan {
namespace process {

boost::filesystem::path find_executable_in_path(
    const boost::filesystem::path &executable) {
  if (executable != executable.filename())
    BOOST_THROW_EXCEPTION(
        non_basename_executable_error()
        << non_basename_executable_error::executable(executable));

  const boost::filesystem::path exts[] = {"", ".exe", ".com", ".bat"};
  for (const auto &ext : exts) {
    TCHAR buf[MAX_PATH];
    LPTSTR dummy;
    const DWORD size = ::SearchPath(nullptr, executable.c_str(), ext.c_str(),
                                    MAX_PATH, buf, &dummy);
    BOOST_ASSERT(size < MAX_PATH);
    if (size > 0) return buf;
  }

  BOOST_THROW_EXCEPTION(no_executable_in_path_error()
                        << no_executable_in_path_error::executable(executable));
}

}  // namespace process
}  // namespace bunsan
