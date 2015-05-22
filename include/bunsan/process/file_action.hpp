#pragma once

#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/variant.hpp>

namespace bunsan{namespace process
{
    enum do_default_type { do_default };
    enum inherit_type { inherit };
    enum suppress_type { suppress };
    enum redirect_to_stdout_type { redirect_to_stdout };

    typedef boost::variant<
        do_default_type,
        inherit_type,
        suppress_type,
        std::string,
        boost::filesystem::path
    > stdin_data_type;

    typedef boost::variant<
        do_default_type,
        inherit_type,
        suppress_type,
        boost::filesystem::path
    > stdout_data_type;

    typedef boost::variant<
        do_default_type,
        inherit_type,
        suppress_type,
        redirect_to_stdout_type,
        boost::filesystem::path
    > stderr_data_type;
}}
