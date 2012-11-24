#include "bunsan/interprocess/sync/file_lock.hpp"

#include <boost/thread/locks.hpp>
#include <boost/assert.hpp>

using namespace bunsan::interprocess;

namespace
{
    file_lock_factory global_file_lock_factory;
}

file_lock_factory &file_lock_factory::instance()
{
    return global_file_lock_factory;
}

const boost::filesystem::path &file_lock_factory::get_path_from_pointer(const mutex_ptr &mtx)
{
    BOOST_ASSERT(mtx);
    return mtx->path;
}

bool file_lock_factory::try_find_mutex(const boost::filesystem::path &path, mutex_ptr &mtx)
{
    index_path_type &by_path = m_instances.get<tag_path>();
    index_path_type::iterator it = by_path.find(path);
    if (it!=by_path.end())
    {
        mtx = *it;
        return true;
    }
    else
        return false;
}

file_lock file_lock_factory::get(const boost::filesystem::path &path)
{
    {
        boost::shared_lock<boost::shared_mutex> lk(m_lock);
        mutex_ptr mtx;
        if (try_find_mutex(path, mtx))
            return file_lock(*this, mtx);
    }
    boost::unique_lock<boost::shared_mutex> lk(m_lock);
    mutex_ptr mtx;
    if (!try_find_mutex(path, mtx))
    {
        mtx.reset(new mutex);
        mtx->path = path;
        mtx->flock = boost::interprocess::file_lock(path.c_str());
        m_instances.insert(mtx);
    }
    return file_lock(*this, mtx);
}

bool file_lock_factory::try_erase(mutex_ptr &mtx)
{
    if (mtx.use_count()==2) // mtx and m_instances
    {
        boost::unique_lock<boost::shared_mutex> lk(m_lock);
        index_pointer_type &by_pointer = m_instances.get<tag_pointer>();
        index_pointer_type::iterator it = by_pointer.find(mtx);
        if (it!=by_pointer.end())
        {
            BOOST_ASSERT(mtx==*it);
            mtx.reset();
            if (it->unique())
            {
                by_pointer.erase(it);
                return true;
            }
            mtx = *it;
        }
    }
    return false;
}

file_lock::file_lock():
    m_factory(0)
{}

file_lock::file_lock(const boost::filesystem::path &path, file_lock_factory &factory_)
{
    *this = factory_.get(path);
}

file_lock::file_lock(file_lock_factory &factory_, const mutex_ptr &mutex_):
    m_factory(&factory_),
    m_mutex(mutex_)
{}

file_lock::file_lock(file_lock &&fl) noexcept:
    m_factory(fl.m_factory),
    m_mutex(std::move(fl.m_mutex))
{
    fl.reset();
}

file_lock &file_lock::operator=(file_lock &&fl) noexcept
{
    this->swap(fl);
    fl.reset();
    return *this;
}

file_lock::~file_lock()
{
    if (*this)
        m_factory->try_erase(m_mutex);
}

void file_lock::reset() noexcept
{
    m_factory = 0;
    m_mutex.reset();
}

file_lock::operator bool() const noexcept
{
    BOOST_ASSERT(static_cast<bool>(m_factory)==static_cast<bool>(m_mutex));
    return static_cast<bool>(m_factory);
}

void file_lock::swap(file_lock &fl) noexcept
{
    using boost::swap;
    swap(m_factory, fl.m_factory);
    swap(m_mutex, fl.m_mutex);
}

void file_lock::lock()
{
    BOOST_ASSERT(*this);
    boost::lock(m_mutex->mlock, m_mutex->flock);
}

bool file_lock::try_lock()
{
    BOOST_ASSERT(*this);
    return boost::try_lock(m_mutex->mlock, m_mutex->flock);
}

bool file_lock::timed_lock(const boost::posix_time::ptime &)
{
    BOOST_ASSERT(*this);
#warning "is not implemented"
    BOOST_ASSERT_MSG(false, "is not implemented");
    return false;
}

void file_lock::unlock()
{
    m_mutex->flock.unlock();
    m_mutex->mlock.unlock();
}

void file_lock::lock_sharable()
{
    BOOST_ASSERT(*this);
#warning "is not implemented"
    BOOST_ASSERT_MSG(false, "is not implemented");
}

bool file_lock::try_lock_sharable()
{
    BOOST_ASSERT(*this);
#warning "is not implemented"
    BOOST_ASSERT_MSG(false, "is not implemented");
    return false;
}

bool file_lock::timed_lock_sharable(const boost::posix_time::ptime &)
{
    BOOST_ASSERT(*this);
#warning "is not implemented"
    BOOST_ASSERT_MSG(false, "is not implemented");
    return false;
}

void file_lock::unlock_sharable()
{
    BOOST_ASSERT(*this);
#warning "is not implemented"
    BOOST_ASSERT_MSG(false, "is not implemented");
}
