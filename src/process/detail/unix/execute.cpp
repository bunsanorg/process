#include <bunsan/process/detail/execute.hpp>

#include "descriptor.hpp"
#include "error.hpp"
#include "executor.hpp"
#include "open.hpp"

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>

#include <iostream>

#include <unistd.h>
#include <sys/wait.h>

namespace bunsan{namespace process{namespace detail
{
    int sync_execute(const context &ctx)
    {
        executor exec_(ctx.executable, ctx.arguments);
        descriptor stdin_fd = open(
            ctx.stdin_file ? *ctx.stdin_file : "/dev/null",
            O_RDWR
        );

        const pid_t pid = ::fork();
        if (pid < 0)
        {
            BOOST_THROW_EXCEPTION(system_error("fork"));
        }
        else if (pid > 0)
        {
            for (;;)
            {
                int stat_loc;
                const pid_t rpid = ::waitpid(pid, &stat_loc, 0);
                if (rpid != pid)
                {
                    BOOST_ASSERT_MSG(rpid < 0, "WNOHANG was not set");
                    if (errno != EINTR)
                    {
                        BOOST_THROW_EXCEPTION(
                            system_error("waitpid") <<
                            system_error::pid(pid));
                    }
                }
                else
                {
                    if (WIFEXITED(stat_loc))
                    {
                        return WEXITSTATUS(stat_loc);
                    }
                    else
                    {
                        BOOST_ASSERT(WIFSIGNALED(stat_loc));
                        return -WTERMSIG(stat_loc);
                    }
                }
            }
        }
        else
        {
            BOOST_ASSERT(pid == 0);
            try
            {
                boost::filesystem::current_path(ctx.current_path);

                {
                    descriptor(STDIN_FILENO).close();
                    stdin_fd = stdin_fd.dup2(STDIN_FILENO);
                    BOOST_ASSERT(*stdin_fd == STDIN_FILENO);
                }

                exec_.exec();
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << std::endl;
                std::abort();
            }
            catch (...)
            {
                std::cerr << "Unknown exception." << std::endl;
                std::abort();
            }
        }
    }
}}}
