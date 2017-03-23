#include "yamss/server/server.hpp"

namespace yamss {
namespace server {

server::server(void* a_context,
                 const std::string& a_endpoint,
                 int a_type,
                 const boost::filesystem::path& a_directory)
  : Yamss::server(a_context, a_endpoint, a_type)
  , m_handler(a_directory)
{
  // empty
}

void
server::advance(const JobKey& a_job)
{
  m_handler.advance(a_job);
}

JobKey
server::create(const std::string& a_url)
{
  return m_handler.create(a_url);
}

void
server::finalize(const JobKey& a_job)
{
  m_handler.finalize(a_job);
}

std::vector<bool>
server::getActiveDofs(const JobKey& a_job)
{
  return m_handler.getActiveDofs(a_job);
}

double
server::getFinalTime(const JobKey& a_job)
{
  return m_handler.getFinalTime(a_job);
}

InterfacePatch
server::getInterface(const JobKey& a_job, const std::int64_t a_loadKey)
{
  auto r = m_handler.getInterface(a_job, a_loadKey);
  return *reinterpret_cast<InterfacePatch*>(&r);
}

InterfaceMovement
server::getMovement(const JobKey& a_job, const int64_t a_loadKey)
{
  auto r = m_handler.getMovement(a_job, a_loadKey);
  return *reinterpret_cast<InterfaceMovement*>(&r);
}

std::vector<double>
server::getModes(const JobKey& a_job)
{
  return m_handler.getModes(a_job);
}

Node
server::getNode(const JobKey& a_job, const int64_t a_nodeKey)
{
  auto r = m_handler.getNode(a_job, a_nodeKey);
  return *reinterpret_cast<Node*>(&r);
}

std::int32_t
server::getNumberOfActiveDofs(const JobKey& a_job)
{
  return m_handler.getNumberOfActiveDofs(a_job);
}

std::int32_t
server::getNumberOfNodes(const JobKey& a_job)
{
  return m_handler.getNumberOfNodes(a_job);
}

State
server::getState(const JobKey& a_job)
{
  auto r = m_handler.getState(a_job);
  return *reinterpret_cast<State*>(&r);
}

double
server::getTime(const JobKey& a_job)
{
  return m_handler.getTime(a_job);
}

double
server::getTimeStep(const JobKey& a_job)
{
  return m_handler.getTimeStep(a_job);
}

void
server::initialize(const JobKey& a_job)
{
  m_handler.initialize(a_job);
}

void
server::release(const JobKey& a_job)
{
  m_handler.release(a_job);
}

void
server::report(const JobKey& a_job)
{
  m_handler.report(a_job);
}

void
server::run(const JobKey& a_job)
{
  m_handler.run(a_job);
}

void
server::runJob(const std::string& a_url)
{
  m_handler.runJob(a_url);
}

void
server::setFinalTime(const JobKey& a_job, const double a_final_time)
{
  m_handler.setFinalTime(a_job, a_final_time);
}

void
server::setLoading(const JobKey& a_job,
                   const int64_t a_load,
                   const InterfaceLoading& a_loading)
{
  auto loading = *reinterpret_cast<const yamss::InterfaceLoading*>(&a_loading);
  m_handler.setLoading(a_job, a_load, loading);
}

void
server::step(const JobKey& a_job)
{
  m_handler.step(a_job);
}

void
server::stepN(const JobKey& a_job, const std::int32_t a_steps)
{
  m_handler.stepN(a_job, a_steps);
}

void
server::subiterate(const JobKey& a_job)
{
  m_handler.subiterate(a_job);
}

} // server namespace
} // yamss namespace
