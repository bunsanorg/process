#include "bunsan/process/execute.hpp"

#include <boost/process.hpp>
#include <boost/filesystem.hpp>

#include "bunsan/util.hpp"

int bunsan::process::sync_execute(bunsan::process::context &&ctx_)
{
    ctx_.build();
    SLOG("trying to execute "<<ctx_.executable()<<" in "<<ctx_.current_path()<<(ctx_.use_path()?" using path":" without using path"));
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
    SLOG("executing \""<<exec_<<"\" in "<<ctx_.current_path()<<" with args");
    for (size_t i = 0; i<ctx_.argv().size(); ++i)
        SLOG("args["<<i<<"] == \""<<ctx_.argv()[i]<<"\"");
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

void bunsan::process::context::build_()
{
    if (argv_.empty() && !executable_)
        throw std::runtime_error("Nothing to execute! Empty argv and executable is not set.");
    else //!argv_.empty() || executable_
    {// we should set missing argv_[0] or executable_
        if (executable_ && argv_.empty())
            argv_.push_back(executable_.get().string());
        else if (!executable_ && !argv_.empty())
            executable_ = argv_.at(0);
    }
    if (!current_path_)
        current_path_ = boost::filesystem::current_path();
    if (!use_path_)// if user haven't set this variable, we have to set it depending executable string
        use_path_ = executable_.get().filename()==executable_.get();
    if (use_path_.get())
    {
        if (executable_.get().empty() || executable_.get()!=executable_.get().filename())// name shouldn't be empty and should be basename
            throw std::runtime_error("You can't try find executable in PATH using empty or non-basename executable name ");
    }
    else// if we do not use path, it is prefered to have absolute path to the executable file or should we raise an exception?
        executable_ = boost::filesystem::absolute(executable_.get());
}
