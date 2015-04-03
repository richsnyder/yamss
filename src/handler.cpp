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
handler::create(JobKey& a_key, const std::string& a_url)
{
  namespace fs = boost::filesystem;

  fs::path key;
  fs::path xml;
  runner_pointer runner_;
  static const std::string model = "%%%%-%%%%-%%%%-%%%%";

  size_type pos = a_url.rfind("/");
  if (pos == std::string::npos)
  {
    YamssException ye;
    boost::format fmt("The URL \"%1%\" is invalid");
    ye.what = boost::str(fmt % a_url);
    throw ye;
  }
  std::string url = a_url.substr(0, pos + 1);

  try
  {
    key = fs::unique_path(model);
    fs::path dir = m_directory / key;
    fs::create_directory(dir);
    xml = dir / "input.xml";
  }
  catch (fs::filesystem_error& e)
  {
    YamssException ye;
    ye.what = "Could not create a directory within which to run a job";
    throw ye;
  }

  try
  {
    m_transporter.get(xml, a_url);
  }
  catch (transport_error& e)
  {
    YamssException ye;
    boost::format fmt("Could not fetch a file at the URL \"%1%\"");
    ye.what = boost::str(fmt % a_url);
    throw ye;
  }

  try
  {
    runner_ = read_input<double>(xml.native());
  }
  catch (std::exception& e)
  {
    YamssException ye;
    boost::format fmt("Could not parse the file at the URL \"%1%\"");
    ye.what = boost::str(fmt % a_url);
    throw ye;
  }

  a_key = key.c_str();
  m_jobs[a_key] = std::make_pair(runner_, url);
}

void
handler::finalize(const JobKey& a_key)
{
  namespace fs = boost::filesystem;

  typename jobs_type::const_iterator job = m_jobs.find(a_key);
  if (job == m_jobs.end())
  {
    YamssException e;
    boost::format fmt("Job %1% does not exist");
    e.what = boost::str(fmt % a_key);
    throw e;
  }
  runner_pointer runner_ = job->second.first;
  std::string url = job->second.second;

  std::set<fs::path> files;
  try
  {
    runner_->finalize();
    files = runner_->get_files();
  }
  catch (std::exception& e)
  {
    YamssException ye;
    boost::format fmt("Failed to finalize the job %1%");
    ye.what = boost::str(fmt % a_key);
    throw ye;
  }

  fs::path local_path;
  std::string remote_url;
  std::set<fs::path>::const_iterator fp;
  try
  {
    for (fp = files.begin(); fp != files.end(); ++fp)
    {
      local_path = m_directory / a_key / *fp;
      remote_url = url + fp->native();
      m_transporter.put(local_path, remote_url);
    }
  }
  catch (transport_error& e)
  {
    YamssException ye;
    ye.what = e.what();
    throw ye;
  }
}

handler::runner_pointer
handler::get_runner(const JobKey& a_key)
{
  typename jobs_type::const_iterator job = m_jobs.find(a_key);
  if (job == m_jobs.end())
  {
    YamssException e;
    boost::format fmt("Job %1% does not exist");
    e.what = boost::str(fmt % a_key);
    throw e;
  }
  return job->second.first;
}

double
handler::getFinalTime(const JobKey& a_key)
{
  return get_runner(a_key)->get_final_time();
}

void
handler::getInterface(Interface& a_interface,
                      const JobKey& a_key,
                      const int64_t a_load_key)
{
  typedef typename runner_type::structure_pointer structure_pointer;
  typedef typename runner_type::structure_type structure_type;
  typedef typename structure_type::load_type load_type;
  typedef typename structure_type::node_type node_type;
  typedef typename structure_type::element_type element_type;
  typedef typename load_type::const_iterator const_iterator;

  int32_t n;
  load_type* load_;
  const_iterator ep;
  const_iterator np;
  const_iterator beg;
  const_iterator end;
  runner_pointer runner_ = get_runner(a_key);
  structure_pointer structure_ = runner_->get_structure();
  boost::unordered_map<key_type, int32_t> node_order;

  key_type key = static_cast<key_type>(a_load_key);
  try
  {
    load_ = &(structure_->get_load(a_load_key));
  }
  catch (std::runtime_error& e)
  {
    YamssException ye;
    ye.what = e.what();
    throw ye;
  }

  size_type n_dofs = structure_->get_number_of_active_dofs();
  size_type n_nodes = load_->get_number_of_nodes();
  size_type n_elements = load_->get_number_of_elements();

  a_interface.activeDofs = structure_->get_active_dofs();
  a_interface.nodeCoordinates.resize(n_dofs * n_nodes);
  a_interface.elementTypes.resize(n_elements);
  a_interface.elementVertices.clear();

  std::vector<int32_t> offsets(6);
  offsets[0] = 0;
  for (n = 1; n < 6; ++n)
  {
    if (structure_->is_active(n))
    {
      offsets[n] = offsets[n - 1] + n_nodes;
    }
    else
    {
      offsets[n] = offsets[n - 1];
    }
  }

  try
  {
    beg = load_->begin_nodes();
    end = load_->end_nodes();
    for (n = 0, np = beg; np != end; ++n, ++np)
    {
      const node_type& node_ = structure_->get_node(*np);
      const typename node_type::vector_type& position = node_.get_position();
      node_order[node_.get_key()] = n;
      for (size_type dof = 0; dof < 6; ++dof)
      {
        if (structure_->is_active(dof))
        {
          a_interface.nodeCoordinates[offsets[dof] + n] = position(n);
        }
      }
    }
  }
  catch (std::runtime_error& e)
  {
    YamssException ye;
    ye.what = e.what();
    throw ye;
  }

  try
  {
    beg = load_->begin_elements();
    end = load_->end_elements();
    for (n = 0, ep = beg; ep != end; ++n, ++ep)
    {
      const element_type& elem_ = structure_->get_element(*ep);
      const typename element_type::vector_type& vertices = elem_.get_vertices();
      switch (elem_.get_shape())
      {
        case element_type::POINT:
          a_interface.elementTypes[n] = ElementType::POINT;
          break;
        case element_type::LINE:
          a_interface.elementTypes[n] = ElementType::LINE;
          break;
        case element_type::TRIANGLE:
          a_interface.elementTypes[n] = ElementType::TRIANGLE;
          break;
        case element_type::QUADRILATERAL:
          a_interface.elementTypes[n] = ElementType::QUADRILATERAL;
          break;
      }
      for (size_type m = 0; m < vertices.size(); ++m)
      {
        a_interface.elementVertices.push_back(vertices[m]);
      }
    }
  }
  catch (std::runtime_error& e)
  {
    YamssException ye;
    ye.what = e.what();
    throw ye;
  }
}

void
handler::getInterfaceMovement(InterfaceMovement& a_movement,
                              const JobKey& a_key,
                              const int64_t a_load_key)
{
  typedef typename runner_type::eom_pointer eom_pointer;
  typedef typename runner_type::eom_type eom_type;
  typedef typename runner_type::structure_pointer structure_pointer;
  typedef typename runner_type::structure_type structure_type;
  typedef typename structure_type::load_type load_type;
  typedef typename structure_type::node_type node_type;
  typedef typename load_type::const_iterator const_iterator;

  int32_t n;
  size_type pos;
  load_type* load_;
  const_iterator np;
  const_iterator beg;
  const_iterator end;
  runner_pointer runner_ = get_runner(a_key);
  eom_pointer eom_ = runner_->get_eom();
  structure_pointer structure_ = runner_->get_structure();

  key_type key = static_cast<key_type>(a_load_key);
  try
  {
    load_ = &(structure_->get_load(a_load_key));
  }
  catch (std::runtime_error& e)
  {
    YamssException ye;
    ye.what = e.what();
    throw ye;
  }

  size_type n_dofs = structure_->get_number_of_active_dofs();
  size_type n_nodes = load_->get_number_of_nodes();

  a_movement.displacements.resize(n_dofs * n_nodes);
  a_movement.velocities.resize(n_dofs * n_nodes);
  a_movement.accelerations.resize(n_dofs * n_nodes);

  std::vector<int32_t> offsets(6);
  offsets[0] = 0;
  for (n = 1; n < 6; ++n)
  {
    if (structure_->is_active(n))
    {
      offsets[n] = offsets[n - 1] + n_nodes;
    }
    else
    {
      offsets[n] = offsets[n - 1];
    }
  }

  typename eom_type::vector_type q = eom_->get_displacement(0);
  typename eom_type::vector_type dq = eom_->get_velocity(0);
  typename eom_type::vector_type ddq = eom_->get_acceleration(0);

  try
  {
    beg = load_->begin_nodes();
    end = load_->end_nodes();
    for (n = 0, np = beg; np != end; ++n, ++np)
    {
      const node_type& node_ = structure_->get_node(*np);
      for (size_type dof = 0; dof < 6; ++dof)
      {
        if (structure_->is_active(dof))
        {
          pos = offsets[dof] + n;
          a_movement.displacements[pos] = node_.get_displacement(dof, q);
          a_movement.velocities[pos] = node_.get_velocity(dof, dq);
          a_movement.accelerations[pos] = node_.get_acceleration(dof, ddq);
        }
      }
    }
  }
  catch (std::runtime_error& e)
  {
    YamssException ye;
    ye.what = e.what();
    throw ye;
  }
}

void
handler::getModes(std::vector<double>& a_modes, const JobKey& a_key)
{
  typedef ::arma::Col<double> vector_type;
  typedef ::arma::conv_to<std::vector<double> > converter;

  runner_pointer runner_ = get_runner(a_key);
  const vector_type& q = runner_->get_eom()->get_displacement(0);
  a_modes = converter::from(q);
}

void
handler::getNode(Node& a_node, const JobKey& a_key, const int64_t a_node_key)
{
  typedef ::arma::Col<double> vector_type;
  typedef ::arma::conv_to<std::vector<double> > converter;
  typedef typename runner_type::structure_type::node_type node_type;

  runner_pointer runner_ = get_runner(a_key);
  key_type key = static_cast<key_type>(a_node_key);
  const node_type& node_ = runner_->get_structure()->get_node(key);
  const vector_type& q = runner_->get_eom()->get_displacement(0);
  const vector_type& x = node_.get_position();
  const vector_type& f = node_.get_force();
  vector_type u = node_.get_displacement(q);
  a_node.position = converter::from(x);
  a_node.displacement = converter::from(u);
  a_node.force = converter::from(f);
}

void
handler::getState(State& a_state, const JobKey& a_key)
{
  typedef ::arma::conv_to<std::vector<double> > converter;

  runner_pointer runner_ = get_runner(a_key);
  typename runner_type::eom_pointer eom_ = runner_->get_eom();
  a_state.step = eom_->get_step(0);
  a_state.time = eom_->get_time(0);
  a_state.displacement = converter::from(eom_->get_displacement(0));
  a_state.velocity = converter::from(eom_->get_velocity(0));
  a_state.acceleration = converter::from(eom_->get_acceleration(0));
  a_state.force = converter::from(eom_->get_force(0));
}

double
handler::getTime(const JobKey& a_key)
{
  return get_runner(a_key)->get_eom()->get_time(0);
}

double
handler::getTimeStep(const JobKey& a_key)
{
  return get_runner(a_key)->get_time_step();
}

void
handler::initialize(const JobKey& a_key)
{
  runner_pointer runner_ = get_runner(a_key);
  try
  {
    runner_->initialize(m_directory / a_key);
  }
  catch (std::exception& e)
  {
    YamssException ye;
    boost::format fmt("Failed to initialize the job %1%");
    ye.what = boost::str(fmt % a_key);
    throw ye;
  }
}

void
handler::release(const JobKey& a_key)
{
  m_jobs.erase(a_key);
}

void
handler::run(const JobKey& a_key)
{
  runner_pointer runner_ = get_runner(a_key);
  try
  {
    runner_->run();
  }
  catch (std::exception& e)
  {
    YamssException ye;
    boost::format fmt("Failed to run the job %1%");
    ye.what = boost::str(fmt % a_key);
    throw ye;
  }
}

void
handler::runJob(const std::string& a_url)
{
  JobKey key;
  this->create(key, a_url);
  this->initialize(key);
  this->run(key);
  this->finalize(key);
  this->release(key);
}

void
handler::setFinalTime(const JobKey& a_key, const double a_final_time)
{
  get_runner(a_key)->set_final_time(a_final_time);
}

void
handler::setInterfaceLoading(const JobKey& a_key,
                             const int64_t a_load_key,
                             const InterfaceLoading& a_loading)
{
  typedef typename runner_type::structure_pointer structure_pointer;
  typedef typename runner_type::structure_type structure_type;
  typedef typename structure_type::load_type load_type;
  typedef typename structure_type::node_type node_type;
  typedef typename load_type::const_iterator const_iterator;

  int32_t n;
  load_type* load_;
  const_iterator np;
  const_iterator beg;
  const_iterator end;
  runner_pointer runner_ = get_runner(a_key);
  structure_pointer structure_ = runner_->get_structure();

  key_type key = static_cast<key_type>(a_load_key);
  try
  {
    load_ = &(structure_->get_load(a_load_key));
  }
  catch (std::runtime_error& e)
  {
    YamssException ye;
    ye.what = e.what();
    throw ye;
  }

  size_type n_dofs = structure_->get_number_of_active_dofs();
  size_type n_nodes = load_->get_number_of_nodes();

  std::vector<int32_t> offsets(6);
  offsets[0] = 0;
  for (n = 1; n < 6; ++n)
  {
    if (structure_->is_active(n))
    {
      offsets[n] = offsets[n - 1] + n_nodes;
    }
    else
    {
      offsets[n] = offsets[n - 1];
    }
  }

  try
  {
    beg = load_->begin_nodes();
    end = load_->end_nodes();
    for (n = 0, np = beg; np != end; ++n, ++np)
    {
      node_type& node_ = structure_->get_node(*np);
      for (size_type dof = 0; dof < 6; ++dof)
      {
        if (structure_->is_active(dof))
        {
          node_.add_force(dof, a_loading.forces[offsets[dof] + n]);
        }
      }
    }
  }
  catch (std::runtime_error& e)
  {
    YamssException ye;
    ye.what = e.what();
    throw ye;
  }
}

void
handler::step(const JobKey& a_key)
{
  runner_pointer runner_ = get_runner(a_key);
  try
  {
    runner_->step();
  }
  catch (std::exception& e)
  {
    YamssException ye;
    boost::format fmt("Failed to step the job %1%");
    ye.what = boost::str(fmt % a_key);
    throw ye;
  }
}

} // server namespace
} // yamss namespace
