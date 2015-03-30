#include "yamss/server/handler.hpp"

namespace yamss {
namespace server {

handler::handler(const path_type& a_working_directory)
  : m_transporter()
  , m_working_directory(a_working_directory)
{
  // empty
}

void
handler::run(const std::string& a_url)
{
  typedef double value_type;
  typedef runner<value_type> runner_type;
  typedef boost::shared_ptr<runner_type> runner_pointer;

  const std::string model = "%%%%-%%%%-%%%%-%%%%";
  boost::filesystem::path casedir = m_working_directory;
  casedir /= boost::filesystem::unique_path(model);
  boost::filesystem::create_directory(casedir);
  path_type input_path = casedir / "input.xml";
  m_transporter.get(input_path, a_url);

  runner_pointer runner_ = read_input<value_type>(input_path.native());
  runner_->initialize(casedir);
  runner_->run();
  runner_->finalize();

  path_type local_path;
  std::string remote_url;
  std::set<path_type>::const_iterator fp;
  std::set<path_type> output_files = runner_->get_files();
  std::string url_base = a_url.substr(0, a_url.rfind("/") + 1);
  for (fp = output_files.begin(); fp != output_files.end(); ++fp)
  {
    local_path = casedir / *fp;
    remote_url = url_base + fp->native();
    m_transporter.put(local_path, remote_url);
  }
}

} // server namespace
} // yamss namespace
