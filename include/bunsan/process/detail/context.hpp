#pragma once

#include <bunsan/process/detail/file_action.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

#include <string>
#include <vector>

namespace bunsan{namespace process{namespace detail
{
    struct context
    {
        boost::filesystem::path executable;
        boost::filesystem::path current_path;
        std::vector<std::string> arguments;
        file_action stdin_file, stdout_file, stderr_file;
    };
}}}
