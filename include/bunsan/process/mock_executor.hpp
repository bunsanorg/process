#include <bunsan/process/executor.hpp>

#include <turtle/mock.hpp>

namespace bunsan{namespace process
{
    MOCK_BASE_CLASS(mock_executor, executor)
    {
        MOCK_METHOD(sync_execute, 1, int (context ctx))
    };
}}
