#include "bunsan/process/error.hpp"

using namespace bunsan::process;

error::error(const std::string &message_): bunsan::error(message_) {}

non_zero_exit_status_error::non_zero_exit_status_error(int exit_status_)
{
    (*this) << exit_status(exit_status_);
}
