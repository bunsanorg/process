#pragma once

#include <bunsan/process/error.hpp>

#include <boost/filesystem/path.hpp>

namespace bunsan{namespace process{namespace detail
{
    struct environment_path_error: virtual error {};
    struct no_environment_path_error: virtual environment_path_error {};
    struct empty_environment_path_error: virtual environment_path_error {};

    struct no_executable_in_path_error: virtual error {};

    boost::filesystem::path find_executable_in_path(
        const boost::filesystem::path &executable);
}}}
