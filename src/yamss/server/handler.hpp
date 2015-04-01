#ifndef YAMSS_SERVER_HANDLER_HPP
#define YAMSS_SERVER_HANDLER_HPP

#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
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

  void
  finalize(const std::string& a_job_key);

  void
  getInterfaces(std::vector<Interface>& a_interfaces,
                const std::string& a_job_key);

  void
  getModes(std::vector<double>& a_modes, const std::string& a_job_key);

  void
  getNode(Node& a_position,
          const std::string& a_job_key,
          const int64_t a_node_key);

  void
  getState(State& a_state, const std::string& a_job_key);

  double
  getTime(const std::string& a_job_key);

  double
  getTimeStep(const std::string& a_job_key);

  void
  initialize(std::string& a_job_key, const std::string& a_url);

  void
  run(const std::string& a_job_key);

  void
  runJob(const std::string& a_url);

  void
  step(const std::string& a_job_key);
protected:
  typedef size_t key_type;
  typedef double value_type;
  typedef ::arma::Col<value_type> vector_type;

  typedef ::yamss::runner<value_type> runner_type;
  typedef ::boost::shared_ptr<runner_type> runner_pointer;
  typedef ::std::pair<runner_pointer, std::string> job_type;

  typedef typename runner_type::eom_pointer eom_pointer;
  typedef typename runner_type::structure_pointer structure_pointer;
  typedef typename runner_type::structure_type structure_type;

  typedef typename structure_type::load_type load_type;
  typedef typename structure_type::node_type node_type;
private:
  ::boost::filesystem::path m_directory;
  ::yamss::server::transporter m_transporter;
  ::boost::unordered_map<std::string, job_type> m_jobs;
}; // handler class

} // server namespace
} // yamss namespace

#endif // YAMSS_SERVER_HANDLER_HPP
