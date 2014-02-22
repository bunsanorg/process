#include <bunsan/process/error.hpp>

#include <sstream>

std::string boost::to_string(
    const bunsan::process::error::arguments &arguments)
{
    std::ostringstream sout;
    sout << '[' << bunsan::error::info_name(arguments) << "] = ";
    bool first = true;
    for (const std::string &argument: arguments.value()) {
        if (!first) {
            sout << ", ";
        }
        sout << argument;
        first = false;
    }
    sout << '\n';
    return sout.str();
}
