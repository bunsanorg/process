#include "bunsan/util.hpp"
#include "bunsan/process/context.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <sstream>

#include <cassert>

int main()
{
    // exceptions
    bunsan::process::context ctx;
    try
    {
        bunsan::process::context ctx;
        ctx.use_path();
    }
    catch (std::invalid_argument &e)
    {
        SLOG("caught: "<<e.what());
    }
    // serialization
    {
        ctx.executable("sdnakdn");
        ctx.argv({"hello", "world"});
    }
    std::stringstream buf;
    {
        boost::archive::text_oarchive oa(buf);
        oa<<ctx;
    }
    {
        bunsan::process::context ctx_;
        boost::archive::text_iarchive ia(buf);
        ia>>ctx_;
        assert(ctx==ctx_);
    }
}
