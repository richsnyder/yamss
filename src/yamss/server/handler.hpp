#ifndef YAMSS_SERVER_HANDLER_HPP
#define YAMSS_SERVER_HANDLER_HPP

#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "yamss/input_reader.hpp"
#include "yamss/runner.hpp"
#include "yamss/evaluator/interface.hpp"
#include "yamss/server/transporter.hpp"
#include "yamss/server/Yamss.h"

namespace yamss {
namespace server {

class handler : virtual public YamssIf
{
public:
  handler(const boost::filesystem::path& a_directory);

  // Job management

  void
  create(JobKey& a_job, const std::string& a_url);

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
  stepN(const JobKey& a_job, const int32_t a_steps);

  void
  run(const JobKey& a_job);

  void
  runJob(const std::string& a_url);

  void
  setFinalTime(const JobKey& a_job, const double a_final_time);

  // Queries

  double
  getTime(const JobKey& a_job);

  double
  getTimeStep(const JobKey& a_job);

  double
  getFinalTime(const JobKey& a_job);

  void
  getModes(std::vector<double>& a_modes, const JobKey& a_job);

  void
  getNode(Node& a_position, const JobKey& a_job, const int64_t a_node_key);

  void
  getState(State& a_state, const JobKey& a_job);

  // Interface methods

  void
  getInterface(Interface& a_interface,
               const JobKey& a_job,
               const int64_t a_load);

  void
  getInterfaceMovement(InterfaceMovement& a_movement,
                       const JobKey& a_job,
                       const int64_t a_load);

  void
  setInterfaceLoading(const JobKey& a_job,
                      const int64_t a_load,
                      const InterfaceLoading& a_loading);
protected:
  typedef size_t key_type;
  typedef size_t size_type;
  typedef ::yamss::runner<double> runner_type;
  typedef ::boost::shared_ptr<runner_type> runner_pointer;
  typedef ::std::pair<runner_pointer, std::string> job_type;

  runner_pointer
  get_runner(const std::string& a_job);
private:
  typedef ::boost::unordered_map<std::string, job_type> jobs_type;

  ::boost::filesystem::path m_directory;
  ::yamss::server::transporter m_transporter;
  jobs_type m_jobs;
}; // handler class

} // server namespace
} // yamss namespace

#endif // YAMSS_SERVER_HANDLER_HPP
