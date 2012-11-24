#pragma once

#include <memory>
#include <atomic>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/global_fun.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/optional.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/filesystem/path.hpp>

namespace bunsan{namespace interprocess
{
    class file_lock;

    class file_lock_factory
    {
    public:
        struct mutex
        {
            boost::filesystem::path path;
            boost::shared_mutex mlock;
            boost::interprocess::file_lock flock;
        };

        typedef std::shared_ptr<mutex> mutex_ptr;

    private:
        static const boost::filesystem::path &get_path_from_pointer(const mutex_ptr &mtx);

    public:
        file_lock_factory()=default;
        file_lock_factory(const file_lock_factory &)=delete;
        file_lock_factory &operator=(const file_lock_factory &)=delete;

        file_lock get(const boost::filesystem::path &path);

        /*!
         * \brief Try to erase mutex from factory
         *
         * Caller should not store any other pointers
         * to the mutex. Argument mtx is reset on success.
         *
         * \return true on success
         */
        bool try_erase(mutex_ptr &mtx);

        /*!
         * \brief per-process instance
         *
         * Do not use before main()
         */
        static file_lock_factory &instance();
    private:
        bool try_find_mutex(const boost::filesystem::path &path, mutex_ptr &mutex);

        boost::shared_mutex m_lock;

        typedef boost::multi_index::multi_index_container
        <
            mutex_ptr,
            boost::multi_index::indexed_by
            <
                boost::multi_index::hashed_unique
                <
                    boost::multi_index::tag<struct tag_path>,
                    boost::multi_index::global_fun
                    <
                        const mutex_ptr &,
                        const boost::filesystem::path &,
                        get_path_from_pointer
                    >
                >,
                boost::multi_index::ordered_unique
                <
                    boost::multi_index::tag<struct tag_pointer>,
                    boost::multi_index::identity<mutex_ptr>
                >
            >
        > mutex_set;
        typedef mutex_set::index<tag_path>::type index_path_type;
        typedef mutex_set::index<tag_pointer>::type index_pointer_type;

        mutex_set m_instances;
    };

    class file_lock
    {
    private:
        typedef file_lock_factory::mutex_ptr mutex_ptr;
    public:
        /// create invalid file_lock
        file_lock();

        file_lock(const boost::filesystem::path &path, file_lock_factory &factory_=file_lock_factory::instance());

        file_lock(file_lock_factory &factory_, const mutex_ptr &mutex_);
        file_lock(file_lock &&lock) noexcept;
        file_lock(const file_lock &)=delete;
        file_lock& operator=(file_lock &&) noexcept;
        ~file_lock();

        void reset() noexcept;
        operator bool() const noexcept;

        void swap(file_lock &) noexcept;
        void lock();
        bool try_lock();
        bool timed_lock(const boost::posix_time::ptime &);
        void unlock();
        void lock_sharable();
        bool try_lock_sharable();
        bool timed_lock_sharable(const boost::posix_time::ptime &);
        void unlock_sharable();

    private:
        file_lock_factory *m_factory;
        mutex_ptr m_mutex;
    };
    inline void swap(file_lock &a, file_lock &b) noexcept
    {
        a.swap(b);
    }
}}
