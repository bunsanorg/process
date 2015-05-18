#include <bunsan/process/executor.hpp>

namespace bunsan{namespace process
{
    class native_executor: public executor
    {
    public:
        native_executor()=default;

        int sync_execute(const context &ctx) override;
    };
}}
