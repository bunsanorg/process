#pragma once

#include "descriptor.hpp"

#include <boost/filesystem/path.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace bunsan {
namespace process {
namespace detail {

descriptor open(const boost::filesystem::path &path, int flags);
descriptor open(const boost::filesystem::path &path, int flags, mode_t mode);

}  // namespace detail
}  // namespace process
}  // namespace bunsan
