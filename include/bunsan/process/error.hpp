#pragma once

#include <bunsan/error.hpp>

#include <boost/filesystem/path.hpp>

namespace bunsan{namespace process
{
    struct error: virtual bunsan::error
    {
        error()=default;

        /// \see bunsan::error::message
        explicit error(const std::string &message_);

        typedef boost::error_info<struct tag_executable, boost::filesystem::path> executable;
    };

    struct non_zero_exit_status_error: virtual error
    {
        non_zero_exit_status_error()=default;

        /// \see exit_status
        explicit non_zero_exit_status_error(int exit_status_);

        /// Program exit status
        typedef boost::error_info<struct tag_exit_status, int> exit_status;
    };

    struct nothing_to_execute_error: virtual error {};

    struct invalid_executable_error: virtual error {};
    struct empty_executable_error: virtual invalid_executable_error {};
    struct non_basename_executable_error: virtual invalid_executable_error {};

    struct context_error: virtual error {};
    struct context_build_error: virtual context_error {};
}}
