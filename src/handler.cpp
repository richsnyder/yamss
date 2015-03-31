#include "yamss/server/handler.hpp"

namespace yamss {
namespace server {

handler::handler(const boost::filesystem::path& a_directory)
  : m_directory(a_directory)
  , m_transporter()
  , m_jobs()
{
  // empty
}

void
handler::finalize(const std::string& a_job_key)
{
  namespace fs = boost::filesystem;

  std::pair<runner_pointer, std::string> job = m_jobs[a_job_key];
  runner_pointer runner_ = job.first;
  std::string url = job.second;

  runner_->finalize();

  fs::path local_path;
  std::string remote_url;
  std::set<fs::path>::const_iterator fp;
  std::set<fs::path> output_files = runner_->get_files();
  for (fp = output_files.begin(); fp != output_files.end(); ++fp)
  {
    local_path = m_directory / a_job_key / *fp;
    remote_url = url + fp->native();
    m_transporter.put(local_path, remote_url);
  }

  m_jobs.erase(a_job_key);
}

void
handler::getModes(std::vector<double>& a_modes,
                  const std::string& a_job_key)
{
  runner_pointer runner_ = m_jobs[a_job_key].first;
  eom_pointer eom_ = runner_->get_eom();
  const vector_type& q = eom_->get_displacement(0);
  a_modes = ::arma::conv_to<std::vector<double> >::from(q);
}

void
handler::getNode(Node& a_node,
                 const std::string& a_job_key,
                 const int64_t a_node_key)
{
  runner_pointer runner_ = m_jobs[a_job_key].first;
  eom_pointer eom_ = runner_->get_eom();
  structure_pointer structure_ = runner_->get_structure();
  const node_type& node_ = structure_->get_node(a_node_key);
  const vector_type& q = eom_->get_displacement(0);
  const vector_type& x = node_.get_position();
  const vector_type& f = node_.get_force();
  vector_type u = node_.get_displacement(q);
  a_node.position = ::arma::conv_to<std::vector<double> >::from(x);
  a_node.displacement = ::arma::conv_to<std::vector<double> >::from(u);
  a_node.force = ::arma::conv_to<std::vector<double> >::from(f);
}

void
handler::getState(State& a_state, const std::string& a_job_key)
{
  runner_pointer runner_ = m_jobs[a_job_key].first;
  eom_pointer eom_ = runner_->get_eom();
  const vector_type& q = eom_->get_displacement(0);
  const vector_type& dq = eom_->get_velocity(0);
  const vector_type& ddq = eom_->get_acceleration(0);
  const vector_type& f = eom_->get_force(0);
  a_state.time = eom_->get_time(0);
  a_state.displacement = ::arma::conv_to<std::vector<double> >::from(q);
  a_state.velocity = ::arma::conv_to<std::vector<double> >::from(dq);
  a_state.acceleration = ::arma::conv_to<std::vector<double> >::from(ddq);
  a_state.force = ::arma::conv_to<std::vector<double> >::from(f);
}

double
handler::getTime(const std::string& a_job_key)
{
  return m_jobs[a_job_key].first->get_eom()->get_time(0);
}

double
handler::getTimeStep(const std::string& a_job_key)
{
  return m_jobs[a_job_key].first->get_time_step();
}

void
handler::initialize(std::string& a_job_key, const std::string& a_url)
{
  namespace fs = boost::filesystem;

  static const std::string model = "%%%%-%%%%-%%%%-%%%%";
  fs::path key = fs::unique_path(model);
  fs::path dir = m_directory / key;
  fs::path xml = dir / "input.xml";
  fs::create_directory(dir);
  m_transporter.get(xml, a_url);

  runner_pointer runner_ = read_input<value_type>(xml.native());
  runner_->initialize(dir);

  std::string url = a_url.substr(0, a_url.rfind("/") + 1);

  a_job_key = key.native();
  m_jobs[a_job_key] = std::make_pair(runner_, url);
}

void
handler::run(const std::string& a_job_key)
{
  m_jobs[a_job_key].first->run();
}

void
handler::runJob(const std::string& a_url)
{
  std::string key;
  this->initialize(key, a_url);
  this->run(key);
  this->finalize(key);
}

void
handler::step(const std::string& a_job_key)
{
  m_jobs[a_job_key].first->step();
}

} // server namespace
} // yamss namespace
