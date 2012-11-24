#include "bunsan/process/execute.hpp"

#include <boost/process.hpp>
#include <boost/filesystem.hpp>

#include "bunsan/util.hpp"

int bunsan::process::sync_execute(const bunsan::process::context &ctx_)
{
    bunsan::process::context ctx__(ctx_);
    return bunsan::process::sync_execute(std::move(ctx__));
}
