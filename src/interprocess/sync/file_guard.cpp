#include "bunsan/interprocess/sync/file_guard.hpp"
#include "bunsan/filesystem/error.hpp"

#include <cstdio>

#include <boost/filesystem/operations.hpp>
#include <boost/assert.hpp>

namespace bunsan{namespace interprocess
{
    file_guard::file_guard(const boost::filesystem::path &path): m_path(path)
    {
        FILE *file = std::fopen(path.c_str(), "wx");
        if (file)
        {
            BOOST_VERIFY(fclose(file) == 0);
        }
        else if (errno == EEXIST)
        {
            BOOST_THROW_EXCEPTION(file_guard_locked_error() <<
                                  file_guard_locked_error::lock_path(path));
        }
        else
        {
            BOOST_THROW_EXCEPTION(filesystem::system_error("open") <<
                                  filesystem::system_error::path(path));
        }
    }

    file_guard::file_guard(file_guard &&guard) noexcept
    {
        swap(guard);
        guard.remove();
    }

    file_guard &file_guard::operator=(file_guard &&guard) noexcept
    {
        swap(guard);
        guard.remove();
        return *this;
    }

    file_guard::operator bool() const noexcept
    {
        return m_path;
    }

    void file_guard::swap(file_guard &guard) noexcept
    {
        using boost::swap;
        swap(m_path, guard.m_path);
    }

    void file_guard::remove()
    {
        if (m_path)
            BOOST_VERIFY(boost::filesystem::remove(m_path.get()));
        m_path.reset();
    }

    file_guard::~file_guard()
    {
        try
        {
            remove();
        }
        catch (...)
        {
            // ignore
        }
    }
}}
