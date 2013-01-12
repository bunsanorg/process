#pragma once

#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/operators.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/serialization/optional.hpp>
#include <boost/serialization/vector.hpp>
#include "bunsan/serialization/path.hpp"

#include "bunsan/get.hpp"

namespace bunsan{namespace process
{
    class context: public boost::equality_comparable<context>
    {
        friend class boost::serialization::access;

#define BUNSAN_PROCESS_CONTEXT_NVP(X) boost::serialization::make_nvp(#X, m_##X)
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int /*version*/)
        {
            ar & BUNSAN_PROCESS_CONTEXT_NVP(current_path);
            ar & BUNSAN_PROCESS_CONTEXT_NVP(executable);
            ar & BUNSAN_PROCESS_CONTEXT_NVP(arguments);
            ar & BUNSAN_PROCESS_CONTEXT_NVP(use_path);
        }
#undef BUNSAN_PROCESS_CONTEXT_NVP

    public:
        context()=default;
        context(const context &)=default;

        inline context(context &&ctx) noexcept
        {
            swap(ctx);
            ctx.reset();
        }

        inline context &operator=(const context &ctx)
        {
            context ctx_(ctx);
            swap(ctx_);
            return *this;
        }

        inline context &operator=(context &&ctx) noexcept
        {
            swap(ctx);
            ctx.reset();
            return *this;
        }

        inline bool operator==(const context &ctx) const
        {
            return m_current_path == ctx.m_current_path &&
                   m_executable == ctx.m_executable &&
                   m_arguments == ctx.m_arguments &&
                   m_use_path == ctx.m_use_path;
        }

        void reset() noexcept
        {
            m_current_path.reset();
            m_executable.reset();
            m_arguments.clear();
            m_use_path.reset();
        }

        inline void swap(context &ctx) noexcept
        {
            using boost::swap;
            swap(m_current_path, ctx.m_current_path);
            swap(m_executable, ctx.m_executable);
            swap(m_arguments, ctx.m_arguments);
            swap(m_use_path, ctx.m_use_path);
        }

        // current path
        inline context &current_path(const boost::filesystem::path &current_path_)
        {
            m_current_path = current_path_;
            return *this;
        }
        inline const boost::filesystem::path &current_path() const
        {
            return get(m_current_path, "current_path member was not initialized");
        }

        // executable
        inline context &executable(const boost::filesystem::path &executable_)
        {
            m_executable = executable_;
            return *this;
        }
        inline const boost::filesystem::path &executable() const
        {
            return get(m_executable, "executable member was not initialized");
        }

        // arguments
        inline context &arguments(std::vector<std::string> &&arguments_)
        {
            m_arguments = std::move(arguments_);
            return *this;
        }
        inline context &arguments(std::vector<std::string> &arguments_)
        {
            m_arguments = arguments_;
            return *this;
        }
        inline context &arguments(const std::vector<std::string> &arguments_)
        {
            m_arguments = arguments_;
            return *this;
        }
        template <typename Arg, typename ... Args>
        inline context &arguments(Arg &&arg, Args &&...args)
        {
            m_arguments.clear();
            arguments_append(std::forward<Arg>(arg), std::forward<Args>(args)...);
            return *this;
        }
        // append
        template <typename Arg1, typename Arg2, typename ... Args>
        inline context &arguments_append(Arg1 &&arg1, Arg2 &&arg2, Args &&...args)
        {
            arguments_append(std::forward<Arg1>(arg1));
            arguments_append(std::forward<Arg2>(arg2), std::forward<Args>(args)...);
            return *this;
        }
        inline context &arguments_append(const char *const obj)
        {
            m_arguments.emplace_back(obj);
            return *this;
        }
        inline context &arguments_append(std::string &&obj)
        {
            m_arguments.emplace_back(std::move(obj));
            return *this;
        }
        inline context &arguments_append(const std::string &obj)
        {
            m_arguments.emplace_back(obj);
            return *this;
        }
        inline context &arguments_append(boost::filesystem::path &&obj)
        {
            arguments_append(obj.string());
            return *this;
        }
        inline context &arguments_append(const boost::filesystem::path &obj)
        {
            arguments_append(obj.string());
            return *this;
        }
        template <typename T>
        inline context &arguments_append(boost::optional<T> &&obj)
        {
            if (obj)
                arguments_append(std::move(obj.get()));
            return *this;
        }
        template <typename T>
        inline context &arguments_append(const boost::optional<T> &obj)
        {
            if (obj)
                arguments_append(obj.get());
            return *this;
        }
        template <typename T>
        inline context &arguments_append(std::vector<T> &&lst)
        {
            for (T &obj: lst)
                arguments_append(std::move(obj));
            return *this;
        }
        template <typename T>
        inline context &arguments_append(const std::vector<T> &lst)
        {
            for (const T &obj: lst)
                arguments_append(obj);
            return *this;
        }
        // end of append
        inline const std::vector<std::string> &arguments() const
        {
            return m_arguments;
        }
        inline std::vector<std::string> &arguments()
        {
            return m_arguments;
        }

        // use path
        inline context &use_path(bool use_path_)
        {
            m_use_path = use_path_;
            return *this;
        }
        inline bool use_path() const
        {
            return get(m_use_path, "use_path member was not initialized");
        }

        // build functions
        /*!
         * \brief prepare context to execution
         *
         * \post all properties are initialized
         */
        inline void build()
        {
            context ctx(*this);
            ctx.build_();
            swap(ctx);
        }

        inline context built() const
        {
            context ctx(*this);
            ctx.build_();
            return ctx;
        }

    private:
        void build_();

        boost::optional<boost::filesystem::path> m_current_path;
        boost::optional<boost::filesystem::path> m_executable;
        std::vector<std::string> m_arguments;
        boost::optional<bool> m_use_path;
    };
    inline void swap(context &a, context &b) noexcept
    {
        a.swap(b);
    }
}}
