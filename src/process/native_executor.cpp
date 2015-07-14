#include <bunsan/process/native_executor.hpp>

#include <bunsan/process/detail/context.hpp>
#include <bunsan/process/detail/execute.hpp>
#include <bunsan/process/detail/file_action.hpp>
#include <bunsan/process/path.hpp>

#include <bunsan/log/trivial.hpp>
#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/tempfile.hpp>

#include <boost/assert.hpp>
#include <boost/io/detail/quoted_manip.hpp>  // for logging
#include <boost/noncopyable.hpp>
#include <boost/variant/static_visitor.hpp>

#include <sstream>  // for logging

namespace bunsan {
namespace process {

namespace {
struct forward_file_action_visitor : boost::static_visitor<detail::file_action>,
                                     private boost::noncopyable {
  detail::file_action operator()(const inherit_type &) { return inherit; }

  detail::file_action operator()(const suppress_type &) { return suppress; }

  detail::file_action operator()(const boost::filesystem::path &path) {
    return path;
  }
};

struct stdin_file_action_visitor : forward_file_action_visitor {
  using forward_file_action_visitor::operator();

  detail::file_action operator()(do_default_type) { return suppress; }

  detail::file_action operator()(const std::string &data) {
    stdin_tmp = tempfile::regular_file_in_tempdir();
    bunsan::filesystem::ofstream fout(stdin_tmp.path());
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout) { fout << data; }
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
    fout.close();
    return stdin_tmp.path();
  }

  tempfile stdin_tmp;
};

struct stdout_file_action_visitor : forward_file_action_visitor {
  using forward_file_action_visitor::operator();

  detail::file_action operator()(do_default_type) { return inherit; }
};

struct stderr_file_action_visitor : forward_file_action_visitor {
  using forward_file_action_visitor::operator();

  detail::file_action operator()(do_default_type) { return inherit; }

  detail::file_action operator()(redirect_to_stdout_type) {
    return detail::stdout_file;
  }
};
}  // namespace

int native_executor::sync_execute(context ctx) {
  ctx.build();
  return sync_execute_impl(ctx);
}

int native_executor::sync_execute_impl(const context &ctx) {
  BUNSAN_LOG_TRACE << "Attempt to execute " << ctx.executable() << " in "
                   << ctx.current_path() << (ctx.use_path() ? " " : " without")
                   << " using path";

  detail::context ctx_;

  if (ctx.use_path())
    ctx_.executable = find_executable_in_path(ctx.executable());
  else
    ctx_.executable = ctx.executable();

  ctx_.current_path = ctx.current_path();
  ctx_.arguments = ctx.arguments();

  stdin_file_action_visitor stdin_visitor;
  stdout_file_action_visitor stdout_visitor;
  stderr_file_action_visitor stderr_visitor;
  ctx_.stdin_file = boost::apply_visitor(stdin_visitor, ctx.stdin_data());
  ctx_.stdout_file = boost::apply_visitor(stdout_visitor, ctx.stdout_data());
  ctx_.stderr_file = boost::apply_visitor(stderr_visitor, ctx.stderr_data());

  {  // begin logging section
    std::ostringstream sout;
    sout << "Executing " << ctx_.executable << " in " << ctx_.current_path
         << " with arguments = [";
    for (std::size_t i = 0; i < ctx_.arguments.size(); ++i) {
      if (i) sout << ", ";
      sout << boost::io::quoted(ctx_.arguments[i]);
    }
    sout << ']';
    BUNSAN_LOG_TRACE << sout.str();
  }  // end logging section

  return detail::sync_execute(ctx_);
}

}  // namespace process
}  // namespace bunsan
