#define BOOST_TEST_MODULE process
#include <boost/test/unit_test.hpp>

#include <bunsan/testing/exec_test.hpp>
#include <bunsan/testing/filesystem/read_data.hpp>
#include <bunsan/testing/filesystem/tempfile.hpp>
#include <bunsan/testing/filesystem/tempfiles.hpp>
#include <bunsan/testing/filesystem/write_data.hpp>

#include <bunsan/process/execute.hpp>

BOOST_AUTO_TEST_SUITE(process)

BOOST_AUTO_TEST_SUITE(unix)

BOOST_FIXTURE_TEST_SUITE(sync_execute, bunsan::testing::filesystem::tempfile)

BOOST_AUTO_TEST_CASE(exit_status)
{
    BOOST_CHECK_EQUAL(
        bunsan::process::sync_execute(
            bunsan::process::context().
                executable("sh").
                arguments("sh", "-c", "exit 10")
        ),
        10
    );

    BOOST_CHECK_EQUAL(
        bunsan::process::sync_execute(
            bunsan::process::context().
                executable("sh").
                arguments("sh", "-c", "kill -10 $$")
        ),
        -10
    );
}

BOOST_AUTO_TEST_CASE(stdin_data)
{
    BOOST_CHECK_EQUAL(
        bunsan::process::sync_execute(
            bunsan::process::context().
                executable("sh").
                stdin_data("exit 11")
        ),
        11
    );

    bunsan::testing::filesystem::write_data(path, "exit 12");
    BOOST_CHECK_EQUAL(
        bunsan::process::sync_execute(
            bunsan::process::context().
                executable("sh").
                stdin_file(path)
        ),
        12
    );

    BOOST_CHECK_EQUAL(
        bunsan::process::sync_execute(
            bunsan::process::context().
                executable("sh").
                arguments("sh", "-c", "read x || exit 15").
                stdin_suppress()
        ),
        15
    );
}

BOOST_AUTO_TEST_CASE(stdin_file)
{
    bunsan::testing::filesystem::write_data(path, "kill -13 $$");
    BOOST_CHECK_EQUAL(
        bunsan::process::sync_execute(
            bunsan::process::context().
                executable("sh").
                stdin_file(path)
        ),
        -13
    );

    BOOST_CHECK_EQUAL(
        bunsan::process::sync_execute(
            bunsan::process::context().
                executable("sh").
                stdin_file(path)
        ),
        -13
    );
}

BOOST_AUTO_TEST_CASE(stdout_file)
{
    BOOST_CHECK_EQUAL(
        bunsan::process::sync_execute(
            bunsan::process::context().
                executable("sh").
                stdin_data("echo \"Hello, world\"").
                stdout_file(path)
        ),
        0
    );
    BOOST_CHECK_EQUAL(
        bunsan::testing::filesystem::read_data(path),
        "Hello, world\n"
    );
}

BOOST_AUTO_TEST_CASE(stderr_file)
{
    BOOST_CHECK_EQUAL(
        bunsan::process::sync_execute(
            bunsan::process::context().
                executable("sh").
                stdin_data("echo \"Hello, world\" >&2").
                stderr_file(path)
        ),
        0
    );
    BOOST_CHECK_EQUAL(
        bunsan::testing::filesystem::read_data(path),
        "Hello, world\n"
    );

    BOOST_CHECK_EQUAL(
        bunsan::process::sync_execute(
            bunsan::process::context().
                executable("sh").
                stdin_data("echo \"Hello, world\" >&2").
                stdout_file(path).
                stderr_redirect_to_stdout()
        ),
        0
    );
    BOOST_CHECK_EQUAL(
        bunsan::testing::filesystem::read_data(path),
        "Hello, world\n"
    );
}

BOOST_FIXTURE_TEST_CASE(std_out_err_file, bunsan::testing::filesystem::tempfiles)
{
    const auto path_out = allocate(), path_err = allocate();
    BOOST_CHECK_EQUAL(
        bunsan::process::sync_execute(
            bunsan::process::context().
                executable("sh").
                stdin_data("echo \"Hello, out world\"\necho \"Hello, err world\" >&2").
                stdout_file(path_out).
                stderr_file(path_err)
        ),
        0
    );
    BOOST_CHECK_EQUAL(
        bunsan::testing::filesystem::read_data(path_out),
        "Hello, out world\n"
    );
    BOOST_CHECK_EQUAL(
        bunsan::testing::filesystem::read_data(path_err),
        "Hello, err world\n"
    );
}

BOOST_AUTO_TEST_SUITE_END() // sync_execute

BOOST_AUTO_TEST_SUITE_END() // unix

BOOST_AUTO_TEST_SUITE_END() // process
