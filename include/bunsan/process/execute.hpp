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
    inline int sync_execute(context &ctx)
    {
        return sync_execute(static_cast<const context &>(ctx));
    }

    // may alter stdout and stderr of context
    int sync_execute_with_output(const context &ctx, std::string &output);
    int sync_execute_with_output(context &&ctx, std::string &output);
    inline int sync_execute_with_output(context &ctx, std::string &output)
    {
        return sync_execute_with_output(static_cast<const context &>(ctx), output);
    }

    void check_sync_execute(const context &ctx);
    void check_sync_execute(context &&ctx);
    inline void check_sync_execute(context &ctx)
    {
        check_sync_execute(static_cast<const context &>(ctx));
    }

    void check_sync_execute_with_output(const context &ctx);
    void check_sync_execute_with_output(context &&ctx);
    inline void check_sync_execute_with_output(context &ctx)
    {
        check_sync_execute_with_output(static_cast<const context &>(ctx));
    }

    // convenient aliases

    template <typename ... Args>
    inline int sync_execute(Args &&...args)
    {
        return sync_execute(
            context().arguments(std::forward<Args>(args)...)
        );
    }

    template <typename ... Args>
    inline void check_sync_execute(Args &&...args)
    {
        check_sync_execute(
            context().arguments(std::forward<Args>(args)...)
        );
    }

    template <typename ... Args>
    inline void check_sync_execute_with_output(Args &&...args)
    {
        check_sync_execute_with_output(
            context().arguments(std::forward<Args>(args)...)
        );
    }
}}
