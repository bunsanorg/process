#pragma once

#include <bunsan/process/file/action.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/variant.hpp>

namespace bunsan {
namespace process {
namespace detail {

enum standard_file { stdin_file, stdout_file, stderr_file };

using file_action = boost::variant<file::inherit_type, file::suppress_type,
                                   standard_file, boost::filesystem::path>;

}  // namespace detail
}  // namespace process
}  // namespace bunsan
