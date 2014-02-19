#pragma once

#include <bunsan/process/error.hpp>

#include <bunsan/system_error.hpp>

#include <sys/types.h>

namespace bunsan{namespace process{namespace detail
{
    struct system_error: bunsan::system_error, virtual process::error
    {
        using bunsan::system_error::system_error;

        typedef boost::error_info<struct tag_pid, pid_t> pid;
    };
}}}
