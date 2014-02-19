#include "descriptor.hpp"

#include <bunsan/system_error.hpp>

#include <unistd.h>

namespace bunsan{namespace process{namespace detail
{
    descriptor::~descriptor()
    {
        close_no_except();
    }

    descriptor::descriptor(const int fd): m_fd(fd) {}

    descriptor::descriptor(descriptor &&o)
    {
        swap(o);
        o.close();
    }

    descriptor &descriptor::operator=(descriptor &&o)
    {
        swap(o);
        o.close();
        return *this;
    }

    void descriptor::reset(const int fd)
    {
        descriptor(fd).swap(*this);
    }

    void descriptor::close()
    {
        if (m_fd)
        {
            const int ret = ::close(*m_fd);
            m_fd = boost::none;
            if (ret)
                BOOST_THROW_EXCEPTION(bunsan::system_error("close"));
        }
    }

    void descriptor::close_no_except() noexcept
    {
        try
        {
            close();
        }
        catch (...)
        {
            // ignore
        }
    }
}}}
