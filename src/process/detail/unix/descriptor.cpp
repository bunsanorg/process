#include "descriptor.hpp"

#include "error.hpp"

#include <boost/assert.hpp>

#include <unistd.h>

namespace bunsan{namespace process{namespace detail
{
    descriptor::~descriptor()
    {
        if (m_close)
            close_no_except();
    }

    descriptor::descriptor(const int fd): m_fd(fd) {}

    descriptor::descriptor(const int fd, const bool close):
        m_fd(fd),
        m_close(close) {}

    descriptor::descriptor(descriptor &&o)
    {
        swap(o);
        o.reset();
    }

    descriptor &descriptor::operator=(descriptor &&o)
    {
        swap(o);
        o.reset();
        return *this;
    }

    void descriptor::reset()
    {
        descriptor().swap(*this);
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

    descriptor descriptor::dup2(const int new_fd) const
    {
        if (!*this)
            BOOST_THROW_EXCEPTION(descriptor_is_closed_error());
        BOOST_ASSERT(m_fd);
        const int fd = ::dup2(*m_fd, new_fd);
        if (fd < 0)
            BOOST_THROW_EXCEPTION(system_error("dup2") <<
                                  system_error::fd(*m_fd) <<
                                  system_error::new_fd(new_fd));
        return descriptor(fd);
    }

    descriptor stdin_descriptor()
    {
        return descriptor(STDIN_FILENO, false);
    }

    descriptor stdout_descriptor()
    {
        return descriptor(STDOUT_FILENO, false);
    }

    descriptor stderr_descriptor()
    {
        return descriptor(STDERR_FILENO, false);
    }
}}}
