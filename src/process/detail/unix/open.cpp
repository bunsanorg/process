#include "open.hpp"

#include "error.hpp"

namespace bunsan{namespace process{namespace detail
{
    descriptor open(const boost::filesystem::path &path, const int flags)
    {
        const int fd = ::open(path.string().c_str(), flags);
        if (fd < 0)
            BOOST_THROW_EXCEPTION(system_error("open") <<
                                  system_error::path(path) <<
                                  system_error::flags(flags));
        return descriptor(fd);
    }

    descriptor open(const boost::filesystem::path &path,
                    const int flags, const mode_t mode)
    {
        const int fd = ::open(path.string().c_str(), flags, mode);
        if (fd < 0)
            BOOST_THROW_EXCEPTION(system_error("open") <<
                                  system_error::path(path) <<
                                  system_error::flags(flags) <<
                                  system_error::mode(mode));
        return descriptor(fd);
    }
}}}
