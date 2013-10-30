#include <bunsan/process/execute.hpp>

#include <bunsan/enable_error_info.hpp>
#include <bunsan/logging/legacy.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/process.hpp>

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
        for (std::size_t i = 0; i < ctx_.arguments().size(); ++i)
            SLOG("args[" << i << "] == \"" << ctx_.arguments()[i] << "\"");
        // waiting
        boost::process::child child = boost::process::launch(exec_, ctx_.arguments(), ctx);
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
    try
    {
        if (m_arguments.empty() && !m_executable)
            BOOST_THROW_EXCEPTION(nothing_to_execute_error() <<
                                  nothing_to_execute_error::message("arguments is empty and executable is not set."));
        else if (m_arguments.empty() && m_executable)
            m_arguments.push_back(m_executable->string());
        else if (!m_arguments.empty() && !m_executable)
            m_executable = m_arguments[0];
        if (!m_current_path)
            m_current_path = boost::filesystem::current_path();
        if (!m_use_path) // if user haven't set this variable, we have to set it depending executable string
            m_use_path = m_executable->filename() == m_executable.get();
        if (m_use_path.get())
        {
            if (m_executable->empty())
                BOOST_THROW_EXCEPTION(empty_executable_error());
            if (m_executable.get() != m_executable->filename())
                BOOST_THROW_EXCEPTION(non_basename_executable_error() <<
                                      non_basename_executable_error::executable(m_executable.get()));
        }
        else
        {
            // TODO if we do not use path, is it prefered to have absolute path to the executable file
            // or should we raise an exception?
            m_executable = boost::filesystem::absolute(m_executable.get());
        }
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(context_build_error() << enable_nested_current());
    }
}
