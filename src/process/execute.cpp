#include <bunsan/process/execute.hpp>

#include <bunsan/process/detail/context.hpp>
#include <bunsan/process/detail/execute.hpp>
#include <bunsan/process/path.hpp>

#include <bunsan/logging/legacy.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/io/detail/quoted_manip.hpp> // for logging

#include <sstream> // for logging

int bunsan::process::sync_execute(bunsan::process::context &&ctx)
{
    ctx.build();
    SLOG("attempt to execute " << ctx.executable() <<
         " in " << ctx.current_path() <<
         (ctx.use_path() ? " ":" without") << " using path");

    detail::context ctx_;

    if (ctx.use_path())
        ctx_.executable = find_executable_in_path(ctx.executable());
    else
        ctx_.executable = ctx.executable();

    ctx_.current_path = ctx.current_path();
    ctx_.arguments = ctx.arguments();

    { // begin logging section
        std::ostringstream sout;
        sout << "executing " << ctx_.executable << " in " <<
                ctx_.current_path << " with arguments = [";
        for (std::size_t i = 0; i < ctx_.arguments.size(); ++i)
        {
            if (i)
                sout << ", ";
            sout << boost::io::quoted(ctx_.arguments[i]);
        }
        sout << ']';
        SLOG(sout.str());
    } // end logging section

    return detail::sync_execute(ctx_);
}

void bunsan::process::context::build_()
{
    try
    {
        if (m_arguments.empty() && !m_executable)
            BOOST_THROW_EXCEPTION(
                nothing_to_execute_error() <<
                nothing_to_execute_error::message(
                    "arguments are empty and executable is not set"));
        else if (m_arguments.empty() && m_executable)
            m_arguments.push_back(m_executable->string());
        else if (!m_arguments.empty() && !m_executable)
            m_executable = m_arguments[0];

        if (!m_current_path)
            m_current_path = boost::filesystem::current_path();

        if (!m_use_path)
            m_use_path = m_executable->filename() == m_executable.get();
        if (m_use_path.get())
        {
            if (m_executable->empty())
                BOOST_THROW_EXCEPTION(empty_executable_error());
            if (m_executable.get() != m_executable->filename())
                BOOST_THROW_EXCEPTION(
                    non_basename_executable_error() <<
                    non_basename_executable_error::executable(m_executable.get()));
        }
        else
        {
            // TODO if we do not use path,
            // is it prefered to have absolute path to the executable file
            // or should we raise an exception?
            m_executable = boost::filesystem::absolute(m_executable.get());
        }
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(context_build_error() <<
                              enable_nested_current());
    }
}
