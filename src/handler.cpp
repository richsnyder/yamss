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

handler::runner_pointer
handler::get_runner(const std::string& a_key)
{
  return m_jobs[a_key].first;
}

void
handler::getInterface(Interface& a_interface,
                      const std::string& a_job_key,
                      const int64_t a_load_key)
{
  typedef ::arma::conv_to<std::vector<double> > converter;

  runner_pointer runner_ = get_runner(a_job_key);
  eom_pointer eom_ = runner_->get_eom();
  structure_pointer structure_ = runner_->get_structure();
  const load_type& load_ = structure_->get_load(a_load_key);
  typename load_type::const_iterator np;
  a_interface.activeDofs = structure_->get_active_dofs();
  a_interface.positions.clear();
  for (np = load_.begin(); np != load_.end(); ++np)
  {
    const vector_type& x = structure_->get_node(*np).get_position();
    a_interface.positions[static_cast<int64_t>(*np)] = converter::from(x);
  }
}

void
handler::getInterfaceNodes(std::map<int64_t, Node>& a_nodes,
                           const std::string& a_job_key,
                           const int64_t a_load_key)
{
  typedef ::arma::conv_to<std::vector<double> > converter;

  Node n;
  key_type key;
  runner_pointer runner_ = get_runner(a_job_key);
  eom_pointer eom_ = runner_->get_eom();
  structure_pointer structure_ = runner_->get_structure();
  const load_type& load_ = structure_->get_load(a_load_key);
  typename load_type::const_iterator np;
  a_nodes.clear();
  const vector_type& q = eom_->get_displacement(0);
  for (np = load_.begin(); np != load_.end(); ++np)
  {
    const node_type& node_ = structure_->get_node(*np);
    const vector_type& x = node_.get_position();
    const vector_type& f = node_.get_force();
    const vector_type u = node_.get_displacement(q);
    n.position = converter::from(x);
    n.displacement = converter::from(u);
    n.force = converter::from(f);
    a_nodes[static_cast<int64_t>(node_.get_key())] = n;
  }
}

void
handler::getModes(std::vector<double>& a_modes,
                  const std::string& a_job_key)
{
  const vector_type& q = get_runner(a_job_key)->get_eom()->get_displacement(0);
  a_modes = ::arma::conv_to<std::vector<double> >::from(q);
}

void
handler::getNode(Node& a_node,
                 const std::string& a_job_key,
                 const int64_t a_node_key)
{
  typedef ::arma::conv_to<std::vector<double> > converter;

  runner_pointer runner_ = get_runner(a_job_key);
  eom_pointer eom_ = runner_->get_eom();
  key_type key = static_cast<key_type>(a_node_key);
  const node_type& node_ = runner_->get_structure()->get_node(key);
  vector_type u = node_.get_displacement(eom_->get_displacement(0));
  a_node.force = converter::from(node_.get_force());
  a_node.position = converter::from(node_.get_position());
  a_node.displacement = converter::from(u);
}

void
handler::getState(State& a_state, const std::string& a_job_key)
{
  typedef ::arma::conv_to<std::vector<double> > converter;

  runner_pointer runner_ = get_runner(a_job_key);
  eom_pointer eom_ = runner_->get_eom();
  a_state.time = eom_->get_time(0);
  a_state.displacement = converter::from(eom_->get_displacement(0));
  a_state.velocity = converter::from(eom_->get_velocity(0));
  a_state.acceleration = converter::from(eom_->get_acceleration(0));
  a_state.force = converter::from(eom_->get_force(0));
}

double
handler::getTime(const std::string& a_job_key)
{
  return get_runner(a_job_key)->get_eom()->get_time(0);
}

double
handler::getTimeStep(const std::string& a_job_key)
{
  return get_runner(a_job_key)->get_time_step();
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
  get_runner(a_job_key)->run();
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
handler::setInterfaceForces(const std::string& a_job_key,
                            const int64_t a_load_key,
                            const std::map<int64_t, std::vector<double> >& a_forces)
{
  typedef ::yamss::evaluator::interface<double> interface_type;
  typedef ::boost::shared_ptr<interface_type> interface_pointer;
  typedef ::arma::conv_to<typename interface_type::vector_type> converter;

  key_type key;
  runner_pointer runner_ = get_runner(a_job_key);
  structure_pointer structure_ = runner_->get_structure();
  std::vector<bool> dofs = structure_->get_active_dofs();
  load_type& load_ = structure_->get_load(static_cast<key_type>(a_load_key));
  typename load_type::evaluator_pointer evaluator_ = load_.get_evaluator();
  interface_pointer interface_ = ::boost::dynamic_pointer_cast<interface_type>(evaluator_);
  std::map<int64_t, std::vector<double> >::const_iterator fp;
  for (fp = a_forces.begin(); fp != a_forces.end(); ++fp)
  {
    key = static_cast<key_type>(fp->first);
    interface_->insert(key, converter::from(fp->second));
  }
}

void
handler::step(const std::string& a_job_key)
{
  get_runner(a_job_key)->step();
}

} // server namespace
} // yamss namespace
