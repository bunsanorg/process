#pragma once

#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>

namespace bunsan{namespace process{namespace detail
{
    class executor: private boost::noncopyable
    {
    public:
        executor(
            const boost::filesystem::path &executable,
            const std::vector<std::string> &arguments);

        [[noreturn]] void exec();

    private:
        const std::string m_executable;
        std::vector<std::string> m_arguments;
        std::vector<char *> m_char_arguments;
    };
}}}
