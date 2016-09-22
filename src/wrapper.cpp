#include "yamss/wrapper/wrapper.hpp"

namespace yamss {
namespace wrapper {

void
advance(const JobKey& a_job) throw(YamssException)
{
  this_handler::get()->advance(a_job);
}

JobKey
create(const std::string& a_url) throw(YamssException)
{
  return this_handler::get()->create(a_url);
}

void
finalize(const JobKey& a_job) throw(YamssException)
{
  this_handler::get()->finalize(a_job);
}

std::vector<bool>
getActiveDofs(const JobKey& a_job) throw(YamssException)
{
  return this_handler::get()->getActiveDofs(a_job);
}

double
getFinalTime(const JobKey& a_job) throw(YamssException)
{
  return this_handler::get()->getFinalTime(a_job);
}

InterfacePatch
getInterface(const JobKey& a_job, const std::int64_t a_loadKey) throw(YamssException)
{
  return this_handler::get()->getInterface(a_job, a_loadKey);
}

InterfaceMovement
getInterfaceMovement(const JobKey& a_job, const int64_t a_loadKey) throw(YamssException)
{
  return this_handler::get()->getInterfaceMovement(a_job, a_loadKey);
}

std::vector<double>
getModes(const JobKey& a_job) throw(YamssException)
{
  return this_handler::get()->getModes(a_job);
}

Node
getNode(const JobKey& a_job, const int64_t a_nodeKey) throw(YamssException)
{
  return this_handler::get()->getNode(a_job, a_nodeKey);
}

std::int32_t
getNumberOfActiveDofs(const JobKey& a_job) throw(YamssException)
{
  return this_handler::get()->getNumberOfActiveDofs(a_job);
}

std::int32_t
getNumberOfNodes(const JobKey& a_job) throw(YamssException)
{
  return this_handler::get()->getNumberOfNodes(a_job);
}

State
getState(const JobKey& a_job) throw(YamssException)
{
  return this_handler::get()->getState(a_job);
}

double
getTime(const JobKey& a_job) throw(YamssException)
{
  return this_handler::get()->getTime(a_job);
}

double
getTimeStep(const JobKey& a_job) throw(YamssException)
{
  return this_handler::get()->getTimeStep(a_job);
}

void
initialize(const JobKey& a_job) throw(YamssException)
{
  this_handler::get()->initialize(a_job);
}

void
release(const JobKey& a_job)
{
  this_handler::get()->release(a_job);
}

void
report(const JobKey& a_job) throw(YamssException)
{
  this_handler::get()->report(a_job);
}

void
run(const JobKey& a_job) throw(YamssException)
{
  this_handler::get()->run(a_job);
}

void
runJob(const std::string& a_url) throw(YamssException)
{
  this_handler::get()->runJob(a_url);
}

void
setFinalTime(const JobKey& a_job, const double a_final_time) throw(YamssException)
{
  this_handler::get()->setFinalTime(a_job, a_final_time);
}

void
setInterfaceLoading(const JobKey& a_job,
                             const int64_t a_load,
                             const InterfaceLoading& a_loading) throw(YamssException)
{
  this_handler::get()->setInterfaceLoading(a_job, a_load, a_loading);
}

void
step(const JobKey& a_job) throw(YamssException)
{
  this_handler::get()->step(a_job);
}

void
stepN(const JobKey& a_job, const std::int32_t a_steps) throw(YamssException)
{
  this_handler::get()->stepN(a_job, a_steps);
}

void
subiterate(const JobKey& a_job) throw(YamssException)
{
  this_handler::get()->subiterate(a_job);
}

} // wrapper namespace
} // yamss namespace
