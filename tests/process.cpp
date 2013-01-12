#define BOOST_TEST_MODULE process
#include <boost/test/unit_test.hpp>

#include "bunsan/process/context.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <sstream>

BOOST_AUTO_TEST_SUITE(process)

BOOST_AUTO_TEST_SUITE(context)

BOOST_AUTO_TEST_CASE(exceptions)
{
    bunsan::process::context ctx;
    BOOST_CHECK_THROW(ctx.use_path(), bunsan::uninitialized_optional_error);
}

BOOST_AUTO_TEST_CASE(serialization)
{
    bunsan::process::context ctx;
    ctx.executable("sdnakdn");
    ctx.arguments({"hello", "world"});

    std::stringstream buf;
    {
        boost::archive::text_oarchive oa(buf);
        oa << ctx;
    }
    {
        bunsan::process::context ctx_;
        boost::archive::text_iarchive ia(buf);
        ia >> ctx_;
        BOOST_CHECK(ctx == ctx_);
    }
}

BOOST_AUTO_TEST_SUITE_END() // context

BOOST_AUTO_TEST_SUITE_END() // process
