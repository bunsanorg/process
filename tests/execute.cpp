#define BOOST_TEST_MODULE execute
#include <boost/test/unit_test.hpp>

#include <bunsan/process/execute.hpp>
#include <bunsan/process/executor.hpp>
#include <bunsan/process/mock_executor.hpp>

namespace bp = bunsan::process;

struct execute_fixture
{
    execute_fixture():
        executor(std::make_shared<bp::mock_executor>())
    {
        bp::executor::register_instance(executor);
    }

    ~execute_fixture()
    {
        bp::executor::register_native();
    }

    std::shared_ptr<bp::mock_executor> executor;
};

BOOST_FIXTURE_TEST_SUITE(execute, execute_fixture)

BOOST_AUTO_TEST_CASE(sync_execute)
{
    MOCK_EXPECT(executor->sync_execute).once().with(
        [](const bp::context &ctx) {
            BOOST_CHECK_EQUAL(ctx.executable(), "hello");
            BOOST_REQUIRE_EQUAL(ctx.arguments().size(), 2);
            BOOST_CHECK_EQUAL(ctx.arguments()[0], "hello");
            BOOST_CHECK_EQUAL(ctx.arguments()[1], "world");
            BOOST_CHECK(ctx.use_path());
            return true;
        }).returns(0);
    bp::sync_execute("hello", "world");
}

BOOST_AUTO_TEST_CASE(check_sync_execute)
{
    MOCK_EXPECT(executor->sync_execute).once().with(mock::any).returns(0);
    MOCK_EXPECT(executor->sync_execute).once().with(mock::any).returns(1);
    BOOST_CHECK_NO_THROW(bp::check_sync_execute("hello"));
    BOOST_CHECK_THROW(bp::check_sync_execute("hello"),
                      bp::non_zero_exit_status_error);
}

BOOST_AUTO_TEST_SUITE_END() // execute
