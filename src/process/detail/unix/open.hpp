#pragma once

#include <bunsan/process/file/handle.hpp>

#include <boost/filesystem/path.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace bunsan {
namespace process {
namespace detail {

file::handle open(const boost::filesystem::path &path, int flags);
file::handle open(const boost::filesystem::path &path, int flags, mode_t mode);

}  // namespace detail
}  // namespace process
}  // namespace bunsan
