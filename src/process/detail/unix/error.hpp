#pragma once

#include <bunsan/process/detail/error.hpp>

#include <bunsan/filesystem/error.hpp>

#include <sys/types.h>

namespace bunsan{namespace process{namespace detail
{
    struct system_error: bunsan::filesystem::system_error, virtual error
    {
        using bunsan::filesystem::system_error::system_error;

        typedef boost::error_info<struct tag_pid, pid_t> pid;
        typedef boost::error_info<struct tag_fd, int> fd;
        typedef boost::error_info<struct tag_new_fd, int> new_fd;
        typedef boost::error_info<struct tag_flags, int> flags;
        typedef boost::error_info<struct tag_mode, mode_t> mode;
    };

    struct descriptor_error: virtual error {};
    struct descriptor_is_closed_error: virtual error {};
}}}
