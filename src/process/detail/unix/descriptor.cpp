#include "descriptor.hpp"

#include "error.hpp"

#include <boost/assert.hpp>

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

    descriptor descriptor::dup() const
    {
        if (!*this)
            BOOST_THROW_EXCEPTION(descriptor_is_closed_error());
        BOOST_ASSERT(m_fd);
        const int fd = ::dup(*m_fd);
        if (fd < 0)
            BOOST_THROW_EXCEPTION(system_error("dup") <<
                                  system_error::fd(*m_fd));
        return descriptor(fd);
    }

    descriptor descriptor::dup2(const int min_fd) const
    {
        if (!*this)
            BOOST_THROW_EXCEPTION(descriptor_is_closed_error());
        BOOST_ASSERT(m_fd);
        const int fd = ::dup2(*m_fd, min_fd);
        if (fd < 0)
            BOOST_THROW_EXCEPTION(system_error("dup2") <<
                                  system_error::fd(*m_fd) <<
                                  system_error::min_fd(min_fd));
        return descriptor(fd);
    }
}}}
