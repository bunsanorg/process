#include "bunsan/process/execute.hpp"
#include "bunsan/enable_error_info.hpp"
#include "bunsan/logging/legacy.hpp"

#include <boost/assert.hpp>
#include <boost/process.hpp>
#include <boost/filesystem/path.hpp>

int bunsan::process::sync_execute(bunsan::process::context &&ctx_)
{
    ctx_.build();
    SLOG("trying to execute " << ctx_.executable() << " in " << ctx_.current_path() << (ctx_.use_path() ? " ":" without") << " using path");
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        boost::process::context ctx;
        ctx.work_directory = ctx_.current_path().string();
        ctx.environment = boost::process::self::get_environment();
        ctx.stdout_behavior = boost::process::inherit_stream();
        ctx.stderr_behavior = boost::process::inherit_stream();
        std::string exec_;
        if (ctx_.use_path())
            exec_ = boost::process::find_executable_in_path(ctx_.executable().string());
        else
            exec_ = ctx_.executable().string();
        SLOG("executing \"" << exec_ << "\" in " << ctx_.current_path() << " with args");
        for (std::size_t i = 0; i < ctx_.argv().size(); ++i)
            SLOG("args[" << i << "] == \"" << ctx_.argv()[i] << "\"");
        // waiting
        boost::process::child child = boost::process::launch(exec_, ctx_.argv(), ctx);
        DLOG(waiting for a child process);
        boost::process::status status = child.wait();
        DLOG(child process has completed);
        if (status.exited())
            return status.exit_status();
        else
            return -1;
    }
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::executable(ctx_.executable()))
}

void bunsan::process::context::build_()
{
    if (argv_.empty() && !executable_)
        BOOST_THROW_EXCEPTION(nothing_to_execute_error() <<
                              nothing_to_execute_error::message("argv is empty and executable is not set."));
    else if (argv_.empty() && executable_)
        argv_.push_back(executable_->string());
    else if (!argv_.empty() && !executable_)
        executable_ = argv_[0];
    if (!current_path_)
        current_path_ = boost::filesystem::current_path();
    if (!use_path_) // if user haven't set this variable, we have to set it depending executable string
        use_path_ = executable_->filename() == executable_.get();
    if (use_path_.get())
    {
        if (executable_->empty())
            BOOST_THROW_EXCEPTION(empty_executable_error());
        if (executable_.get() != executable_->filename())
            BOOST_THROW_EXCEPTION(non_basename_executable_error() <<
                                  non_basename_executable_error::executable(executable_.get()));
    }
    else
    {
        // TODO if we do not use path, is it prefered to have absolute path to the executable file
        // or should we raise an exception?
        executable_ = boost::filesystem::absolute(executable_.get());
    }
}
