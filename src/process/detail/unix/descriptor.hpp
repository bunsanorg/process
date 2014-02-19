#pragma once

#include <boost/optional.hpp>

namespace bunsan{namespace process{namespace detail
{
    class descriptor
    {
    public:
        descriptor()=default;
        ~descriptor();

        explicit descriptor(const int fd);

        descriptor(const descriptor &)=delete;
        descriptor(descriptor &&);

        descriptor &operator=(const descriptor &)=delete;
        descriptor &operator=(descriptor &&);

        explicit inline operator bool() const { return m_fd; }
        inline int operator*() const { return *m_fd; }

        void reset(const int fd);
        void close();
        void close_no_except() noexcept;

        inline void swap(descriptor &o) noexcept { m_fd.swap(o.m_fd); }

    private:
        boost::optional<int> m_fd;
    };
    inline void swap(descriptor &a, descriptor &b) noexcept
    {
        a.swap(b);
    }
}}}
