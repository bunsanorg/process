#include <bunsan/process/execute.hpp>

int bunsan::process::sync_execute(const bunsan::process::context &ctx_)
{
    bunsan::process::context ctx__(ctx_);
    return bunsan::process::sync_execute(std::move(ctx__));
}
