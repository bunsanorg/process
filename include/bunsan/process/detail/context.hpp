#pragma once

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>

namespace bunsan{namespace process{namespace detail
{
    struct context
    {
        boost::filesystem::path executable;
        boost::filesystem::path current_path;
        std::vector<std::string> arguments;
    };
}}}
