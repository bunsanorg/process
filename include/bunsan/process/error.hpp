#pragma once

#include "bunsan/error.hpp"

namespace bunsan{namespace process
{
    struct error: virtual bunsan::error
    {
        error()=default;

        /// \see bunsan::error::message
        explicit error(const std::string &message_);
    };

    struct non_zero_exit_status_error: public virtual error
    {
        non_zero_exit_status_error()=default;

        /// \see exit_status
        explicit non_zero_exit_status_error(int exit_status_);

        /// Program exit status
        typedef boost::error_info<struct tag_exit_status, int> exit_status;
    };
}}
