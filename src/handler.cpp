#include "yamss/handler.hpp"

namespace yamss {

handler::handler(const boost::filesystem::path& a_directory)
  : m_directory(a_directory)
  , m_transporter()
  , m_jobs()
{
  // empty
}

void
handler::advance(const JobKey& a_job)
{
  return get_runner(a_job)->advance();
}

JobKey
handler::create(const std::string& a_url)
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

  std::string job = key.c_str();
  m_jobs[job] = std::make_pair(runner_, url);
  return job;
}

void
handler::finalize(const JobKey& a_job)
{
  namespace fs = boost::filesystem;

  typename jobs_type::const_iterator job = m_jobs.find(a_job);
  if (job == m_jobs.end())
  {
    YamssException ye;
    boost::format fmt("Job %1% does not exist");
    ye.what = boost::str(fmt % a_job);
    throw ye;
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
    ye.what = boost::str(fmt % a_job);
    throw ye;
  }

  if (url.substr(0, 4) == "file")
  {
    fs::path local_path;
    std::string remote_url;
    std::set<fs::path>::const_iterator fp;
    try
    {
      for (fp = files.begin(); fp != files.end(); ++fp)
      {
        local_path = m_directory / a_job / *fp;
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
}

handler::runner_pointer
handler::get_runner(const JobKey& a_job)
{
  typename jobs_type::const_iterator job = m_jobs.find(a_job);
  if (job == m_jobs.end())
  {
    YamssException ye;
    boost::format fmt("Job %1% does not exist");
    ye.what = boost::str(fmt % a_job);
    throw ye;
  }
  return job->second.first;
}

std::vector<bool>
handler::getActiveDofs(const JobKey& a_job)
{
  return get_runner(a_job)->get_structure()->get_active_dofs();
}

double
handler::getFinalTime(const JobKey& a_job)
{
  return get_runner(a_job)->get_final_time();
}

InterfacePatch
handler::getInterface(const JobKey& a_job, const std::int64_t a_loadKey)
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
  runner_pointer runner_ = get_runner(a_job);
  structure_pointer structure_ = runner_->get_structure();
  boost::unordered_map<key_type, int32_t> node_order;

  key_type key = static_cast<key_type>(a_loadKey);
  try
  {
    load_ = &(structure_->get_load(a_loadKey));
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

  InterfacePatch patch;
  patch.activeDofs = structure_->get_active_dofs();
  patch.nodeCoordinates.resize(n_dofs * n_nodes);
  patch.elementTypes.resize(n_elements);
  patch.elementVertices.clear();

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
          patch.nodeCoordinates[offsets[dof] + n] = position(n);
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
          patch.elementTypes[n] = ElementType::POINT;
          break;
        case element_type::LINE:
          patch.elementTypes[n] = ElementType::LINE;
          break;
        case element_type::TRIANGLE:
          patch.elementTypes[n] = ElementType::TRIANGLE;
          break;
        case element_type::QUADRILATERAL:
          patch.elementTypes[n] = ElementType::QUADRILATERAL;
          break;
      }
      for (size_type m = 0; m < vertices.size(); ++m)
      {
        patch.elementVertices.push_back(vertices[m]);
      }
    }
  }
  catch (std::runtime_error& e)
  {
    YamssException ye;
    ye.what = e.what();
    throw ye;
  }

  return patch;
}

InterfaceMovement
handler::getInterfaceMovement(const JobKey& a_job, const int64_t a_loadKey)
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
  runner_pointer runner_ = get_runner(a_job);
  eom_pointer eom_ = runner_->get_eom();
  structure_pointer structure_ = runner_->get_structure();

  key_type key = static_cast<key_type>(a_loadKey);
  try
  {
    load_ = &(structure_->get_load(a_loadKey));
  }
  catch (std::runtime_error& e)
  {
    YamssException ye;
    ye.what = e.what();
    throw ye;
  }

  size_type n_dofs = structure_->get_number_of_active_dofs();
  size_type n_nodes = load_->get_number_of_nodes();

  InterfaceMovement movement;
  movement.displacements.resize(n_dofs * n_nodes);
  movement.velocities.resize(n_dofs * n_nodes);
  movement.accelerations.resize(n_dofs * n_nodes);

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
          movement.displacements[pos] = node_.get_displacement(dof, q);
          movement.velocities[pos] = node_.get_velocity(dof, dq);
          movement.accelerations[pos] = node_.get_acceleration(dof, ddq);
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

  return movement;
}

std::vector<double>
handler::getModes(const JobKey& a_job)
{
  typedef ::arma::Col<double> vector_type;
  typedef ::arma::conv_to<std::vector<double> > converter;

  runner_pointer runner_ = get_runner(a_job);
  const vector_type& q = runner_->get_eom()->get_displacement(0);
  return converter::from(q);
}

Node
handler::getNode(const JobKey& a_job, const int64_t a_nodeKey)
{
  typedef ::arma::Col<double> vector_type;
  typedef ::arma::conv_to<std::vector<double> > converter;
  typedef typename runner_type::structure_type::node_type node_type;

  runner_pointer runner_ = get_runner(a_job);
  key_type key = static_cast<key_type>(a_nodeKey);
  const node_type& node_ = runner_->get_structure()->get_node(key);
  const vector_type& q = runner_->get_eom()->get_displacement(0);
  const vector_type& x = node_.get_position();
  const vector_type& f = node_.get_force();
  vector_type u = node_.get_displacement(q);

  Node node;
  node.position = converter::from(x);
  node.displacement = converter::from(u);
  node.force = converter::from(f);
  return node;
}

std::int32_t
handler::getNumberOfActiveDofs(const JobKey& a_job)
{
  return get_runner(a_job)->get_structure()->get_number_of_active_dofs();
}

std::int32_t
handler::getNumberOfNodes(const JobKey& a_job)
{
  return get_runner(a_job)->get_structure()->get_number_of_nodes();
}

State
handler::getState(const JobKey& a_job)
{
  typedef ::arma::conv_to<std::vector<double> > converter;

  runner_pointer runner_ = get_runner(a_job);
  typename runner_type::eom_pointer eom_ = runner_->get_eom();

  State state;
  state.step = eom_->get_step(0);
  state.time = eom_->get_time(0);
  state.displacement = converter::from(eom_->get_displacement(0));
  state.velocity = converter::from(eom_->get_velocity(0));
  state.acceleration = converter::from(eom_->get_acceleration(0));
  state.force = converter::from(eom_->get_force(0));
  return state;
}

double
handler::getTime(const JobKey& a_job)
{
  return get_runner(a_job)->get_eom()->get_time(0);
}

double
handler::getTimeStep(const JobKey& a_job)
{
  return get_runner(a_job)->get_time_step();
}

void
handler::initialize(const JobKey& a_job)
{
  runner_pointer runner_ = get_runner(a_job);
  try
  {
    runner_->initialize(m_directory / a_job);
  }
  catch (std::exception& e)
  {
    YamssException ye;
    boost::format fmt("Failed to initialize the job %1%");
    ye.what = boost::str(fmt % a_job);
    throw ye;
  }
}

void
handler::release(const JobKey& a_job)
{
  m_jobs.erase(a_job);
}

void
handler::report(const JobKey& a_job)
{
  return get_runner(a_job)->report();
}

void
handler::run(const JobKey& a_job)
{
  runner_pointer runner_ = get_runner(a_job);
  try
  {
    runner_->run();
  }
  catch (std::exception& e)
  {
    YamssException ye;
    boost::format fmt("Failed to run the job %1%");
    ye.what = boost::str(fmt % a_job);
    throw ye;
  }
}

void
handler::runJob(const std::string& a_url)
{
  JobKey key = this->create(a_url);
  this->initialize(key);
  this->run(key);
  this->finalize(key);
  this->release(key);
}

void
handler::setFinalTime(const JobKey& a_job, const double a_final_time)
{
  get_runner(a_job)->set_final_time(a_final_time);
}

void
handler::setInterfaceLoading(const JobKey& a_job,
                             const int64_t a_load,
                             const InterfaceLoading& a_loading)
{
  typedef typename runner_type::structure_pointer structure_pointer;
  typedef typename runner_type::structure_type structure_type;
  typedef typename structure_type::load_type load_type;
  typedef typename structure_type::node_type node_type;
  typedef typename load_type::const_iterator const_iterator;
  typedef typename evaluator::interface<double> interface_type;
  typedef boost::shared_ptr<interface_type> interface_pointer;

  int32_t n;
  load_type* load_;
  const_iterator np;
  const_iterator beg;
  const_iterator end;
  runner_pointer runner_ = get_runner(a_job);
  structure_pointer structure_ = runner_->get_structure();

  key_type key = static_cast<key_type>(a_load);
  try
  {
    load_ = &(structure_->get_load(a_load));
  }
  catch (std::runtime_error& e)
  {
    YamssException ye;
    ye.what = e.what();
    throw ye;
  }

  std::vector<double> f(6);
  interface_pointer evaluator_ = boost::dynamic_pointer_cast<interface_type>(
      load_->get_evaluator()
    );

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
          f[dof] = a_loading.forces[offsets[dof] + n];
        }
        else
        {
          f[dof] = 0.0;
        }
      }
      evaluator_->insert(node_.get_key(), f);
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
handler::step(const JobKey& a_job)
{
  runner_pointer runner_ = get_runner(a_job);
  try
  {
    runner_->step();
  }
  catch (std::exception& e)
  {
    YamssException ye;
    boost::format fmt("Failed to step the job %1%");
    ye.what = boost::str(fmt % a_job);
    throw ye;
  }
}

void
handler::stepN(const JobKey& a_job, const std::int32_t a_steps)
{
  runner_pointer runner_ = get_runner(a_job);
  try
  {
    for (size_type n = 0; n < a_steps; ++n)
    {
      runner_->step();
    }
  }
  catch (std::exception& e)
  {
    YamssException ye;
    boost::format fmt("Failed to step the job %1%");
    ye.what = boost::str(fmt % a_job);
    throw ye;
  }
}

void
handler::subiterate(const JobKey& a_job)
{
  return get_runner(a_job)->subiterate();
}

} // yamss namespace
