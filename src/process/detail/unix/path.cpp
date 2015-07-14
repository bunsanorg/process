#include <bunsan/process/detail/path.hpp>

#include <bunsan/process/error.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/operations.hpp>

#include <cstdlib>

namespace bunsan {
namespace process {
namespace detail {

boost::filesystem::path find_executable_in_path(
    const boost::filesystem::path &executable) {
  if (executable != executable.filename())
    BOOST_THROW_EXCEPTION(
        non_basename_executable_error()
        << non_basename_executable_error::executable(executable));

  const char *const envpath = ::getenv("PATH");
  if (!envpath) BOOST_THROW_EXCEPTION(no_environment_path_error());

  /*const*/ std::string path(envpath);
  if (path.empty()) BOOST_THROW_EXCEPTION(empty_environment_path_error());

  std::vector<std::string> path_dirs;
  boost::algorithm::split(path_dirs, path, boost::algorithm::is_any_of(":"),
                          boost::algorithm::token_compress_on);

  for (const boost::filesystem::path dir : path_dirs) {
    const boost::filesystem::path abs_path =
        boost::filesystem::absolute(dir / executable);
    if (boost::filesystem::exists(abs_path)) return abs_path;
  }

  BOOST_THROW_EXCEPTION(no_executable_in_path_error()
                        << no_executable_in_path_error::executable(executable));
}

}  // namespace detail
}  // namespace process
}  // namespace bunsan
