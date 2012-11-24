#pragma once

#include <string>
#include <vector>
#include <functional>

#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>

#include "bunsan/process/error.hpp"
#include "bunsan/process/context.hpp"

namespace bunsan
{
    namespace process
    {
        int sync_execute(const context &ctx);

        int sync_execute(context &&ctx);

        inline int sync_execute(
            const boost::filesystem::path &cwd,
            const boost::filesystem::path &executable,
            const std::vector<std::string> &args,
            bool use_path=true)
        {
            return sync_execute(context().current_path(cwd).executable(executable).argv(args).use_path(use_path));
        }

        inline int sync_execute(
            const boost::filesystem::path &cwd,
            const std::vector<std::string> &args,
            bool use_path=true)
        {
            return sync_execute(context().current_path(cwd).argv(args).use_path(use_path));
        }

        template <typename ... Args>
        void check_sync_execute(Args &&...args)
        {
            int exit_status_ = sync_execute(std::forward<Args>(args)...);
            if (exit_status_)
                BOOST_THROW_EXCEPTION(bunsan::process::non_zero_exit_status_error(exit_status_));
        }
    }
}
