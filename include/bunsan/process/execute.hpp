#pragma once

#include <bunsan/process/context.hpp>
#include <bunsan/process/error.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

#include <string>
#include <utility>
#include <vector>

namespace bunsan{namespace process
{
    int sync_execute(const context &ctx);

    int sync_execute(context &&ctx);

    inline int sync_execute(
        const boost::filesystem::path &current_path,
        const boost::filesystem::path &executable,
        const std::vector<std::string> &arguments,
        bool use_path=true)
    {
        return sync_execute(
            context().
            current_path(current_path).
            executable(executable).
            arguments(arguments).
            use_path(use_path)
        );
    }

    inline int sync_execute(
        const boost::filesystem::path &current_path,
        const std::vector<std::string> &arguments,
        bool use_path=true)
    {
        return sync_execute(
            context().
            current_path(current_path).
            arguments(arguments).
            use_path(use_path)
        );
    }

    template <typename ... Args>
    void check_sync_execute(Args &&...args)
    {
        const int exit_status_ = sync_execute(std::forward<Args>(args)...);
        if (exit_status_)
            BOOST_THROW_EXCEPTION(
                bunsan::process::non_zero_exit_status_error(exit_status_));
    }
}}
