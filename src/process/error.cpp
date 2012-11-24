#include "bunsan/process/error.hpp"

#include <boost/format.hpp>

using namespace bunsan::process;

error::error(const std::string &message_): bunsan::error(message_) {}

non_zero_exit_status_error::non_zero_exit_status_error(int exit_status_):
    error((boost::format("Program has completed with non-zero exit status = %1%") % exit_status_).str())
{
    (*this)<<exit_status(exit_status_);
}
