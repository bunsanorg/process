#pragma once

#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/vector.hpp>

#include "bunsan/serialization/path.hpp"
#include "bunsan/util.hpp"

namespace bunsan
{
    namespace process
    {
        class context
        {
        friend class boost::serialization::access;

        template <typename Archive>
        void serialize(Archive &ar, const unsigned int /*version*/)
        {
            ar & current_path_;
            ar & executable_;
            ar & argv_;
            ar & use_path_;
        }

        public:
            context()=default;
            context(const context &)=default;

            inline context(context &&ctx):  current_path_(std::move(ctx.current_path_)),
                            executable_(std::move(ctx.executable_)),
                            argv_(std::move(ctx.argv_)),
                            use_path_(std::move(ctx.use_path_)){}

            context &operator=(const context &ctx)
            {
                context ctx_(ctx);
                this->swap(ctx_);
                return *this;
            }

            inline context &operator=(context &&ctx) noexcept
            {
                this->swap(ctx);
                return *this;
            }

            inline bool operator==(const context &ctx) const
            {
                return  current_path_==ctx.current_path_ &&
                        executable_==ctx.executable_ &&
                        argv_==ctx.argv_ &&
                        use_path_==ctx.use_path_;
            }

            inline void swap(context &ctx) noexcept
            {
                using boost::swap;
                swap(current_path_, ctx.current_path_);
                swap(executable_, ctx.executable_);
                swap(argv_, ctx.argv_);
                swap(use_path_, ctx.use_path_);
            }

            // current path
            inline context &current_path(const boost::filesystem::path &current_path_)
            {
                this->current_path_ = current_path_;
                return *this;
            }
            inline const boost::filesystem::path &current_path() const
            {
                return bunsan::get(current_path_, "current_path member was not initialized");
            }

            // executable
            inline context &executable(const boost::filesystem::path &executable_)
            {
                this->executable_ = executable_;
                return *this;
            }
            inline const boost::filesystem::path &executable() const
            {
                return bunsan::get(executable_, "executable member was not initialized");
            }

            // argv
            inline context &argv(const std::vector<std::string> &argv_)
            {
                this->argv_ = argv_;
                return *this;
            }
            inline const std::vector<std::string> &argv() const
            {
                return argv_;
            }
            inline std::vector<std::string> &argv()
            {
                return argv_;
            }

            // use path
            inline context &use_path(bool use_path_)
            {
                this->use_path_ = use_path_;
                return *this;
            }
            inline bool use_path() const
            {
                return bunsan::get(use_path_, "use_path member was not initialized");
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
                this->swap(ctx);
            }

            inline context built() const
            {
                context ctx(*this);
                ctx.build_();
                return ctx;
            }

        private:
            void build_();

            boost::optional<boost::filesystem::path> current_path_;
            boost::optional<boost::filesystem::path> executable_;
            std::vector<std::string> argv_;
            boost::optional<bool> use_path_;
        };
        inline void swap(context &a, context &b) noexcept
        {
            a.swap(b);
        }
    }
}
