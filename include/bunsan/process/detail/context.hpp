#pragma once

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
        boost::optional<boost::filesystem::path> stdin_file;
    };
}}}
