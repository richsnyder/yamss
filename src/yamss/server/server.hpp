#ifndef YAMSS_SERVER_SERVER_HPP
#define YAMSS_SERVER_SERVER_HPP

#include "yamss/handler.hpp"
#include "yamss/server/yamss.hpp"

namespace yamss {
namespace server {

class server : public Yamss::server
{
public:
  server(void* a_context,
         const std::string& a_endpoint,
         int a_type,
         const boost::filesystem::path& a_directory);

  // Job management

  JobKey
  create(const std::string& a_url);

  void
  release(const JobKey& a_job);

  // Simulation control

  void
  initialize(const JobKey& a_job);

  void
  finalize(const JobKey& a_job);

  void
  step(const JobKey& a_job);

  void
  stepN(const JobKey& a_job, const std::int32_t a_steps);

  void
  advance(const JobKey& a_job);

  void
  subiterate(const JobKey& a_job);

  void
  report(const JobKey& a_job);

  void
  run(const JobKey& a_job);

  void
  runJob(const std::string& a_url);

  void
  setFinalTime(const JobKey& a_job, const double a_final_time);

  // Queries

  std::vector<bool>
  getActiveDofs(const JobKey& a_job);

  std::int32_t
  getNumberOfActiveDofs(const JobKey& a_job);

  std::int32_t
  getNumberOfNodes(const JobKey& a_job);

  double
  getTime(const JobKey& a_job);

  double
  getTimeStep(const JobKey& a_job);

  double
  getFinalTime(const JobKey& a_job);

  std::vector<double>
  getModes(const JobKey& a_job);

  Node
  getNode(const JobKey& a_job, const std::int64_t a_nodeKey);

  State
  getState(const JobKey& a_job);

  // Interface methods

  InterfacePatch
  getInterface(const JobKey& a_job, const std::int64_t a_loadKey);

  InterfaceMovement
  getMovement(const JobKey& a_job, const std::int64_t a_loadKey);

  void
  setLoading(const JobKey& a_job,
             const std::int64_t a_loadKey,
             const InterfaceLoading& a_loading);
private:
  handler m_handler;
}; // server class

} // server namespace
} // yamss namespace

#endif // YAMSS_SERVER_SERVER_HPP
