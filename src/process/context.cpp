#include <bunsan/process/context.hpp>

#include <sstream>

namespace boost
{

#define BUNSAN_PROCESS_CONTEXT_PRINT(FIELD) \
    if (first_field) \
        first_field = false; \
    else \
        sout << ", "; \
    sout << #FIELD << " = ";

    std::string to_string(const bunsan::process::context &context)
    {
        std::ostringstream sout;
        sout << "{ ";
        bool first_field = true;

        if (context.m_executable)
        {
            BUNSAN_PROCESS_CONTEXT_PRINT(executable);
            sout << context.m_executable;
        }

        // arguments
        {
            BUNSAN_PROCESS_CONTEXT_PRINT(arguments);
            sout << '[';
            for (std::size_t i = 0; i < context.m_arguments.size(); ++i)
            {
                if (i)
                    sout << ", ";
                else
                    sout << ' ';
                sout << boost::io::quoted(context.m_arguments[i]);
            }
            sout << " ]";
        }

        if (!first_field)
            sout << ' ';
        sout << '}';

        return sout.str();
    }
}
