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
  create(JobKey& a_key, const std::string& a_url);

  void
  release(const JobKey& a_key);

  // Simulation control

  void
  initialize(const JobKey& a_key);

  void
  finalize(const JobKey& a_key);

  void
  step(const JobKey& a_key);

  void
  run(const JobKey& a_key);

  void
  runJob(const std::string& a_url);

  void
  setFinalTime(const JobKey& a_key, const double a_final_time);

  // Queries

  double
  getTime(const JobKey& a_key);

  double
  getTimeStep(const JobKey& a_key);

  double
  getFinalTime(const JobKey& a_key);

  void
  getModes(std::vector<double>& a_modes, const JobKey& a_key);

  void
  getNode(Node& a_position, const JobKey& a_key, const int64_t a_node_key);

  void
  getState(State& a_state, const JobKey& a_key);

  // Interface methods

  void
  getInterface(Interface& a_interface,
               const JobKey& a_key,
               const int64_t a_load_key);

  void
  getInterfaceMovement(InterfaceMovement& a_movement,
                       const JobKey& a_key,
                       const int64_t a_load_key);

  void
  setInterfaceLoading(const JobKey& a_key,
                      const int64_t a_load_key,
                      const InterfaceLoading& a_loading);
protected:
  typedef size_t key_type;
  typedef size_t size_type;
  typedef ::yamss::runner<double> runner_type;
  typedef ::boost::shared_ptr<runner_type> runner_pointer;
  typedef ::std::pair<runner_pointer, std::string> job_type;

  runner_pointer
  get_runner(const std::string& a_key);
private:
  typedef ::boost::unordered_map<std::string, job_type> jobs_type;

  ::boost::filesystem::path m_directory;
  ::yamss::server::transporter m_transporter;
  jobs_type m_jobs;
}; // handler class

} // server namespace
} // yamss namespace

#endif // YAMSS_SERVER_HANDLER_HPP
