#ifndef YAMSS_HANDLER_HPP
#define YAMSS_HANDLER_HPP

#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "yamss/input_reader.hpp"
#include "yamss/runner.hpp"
#include "yamss/transporter.hpp"
#include "yamss/evaluator/interface.hpp"

namespace yamss {

enum ElementType
{
  POINT,
  LINE,
  TRIANGLE,
  QUADRILATERAL
};

typedef std::string JobKey;

struct InterfaceLoading
{
  std::vector<double> forces;
};

struct InterfaceMovement
{
  std::vector<double> displacements;
  std::vector<double> velocities;
  std::vector<double> accelerations;
};

struct InterfacePatch
{
  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> z;
  std::vector<ElementType> elementTypes;
  std::vector<std::int32_t> elementVertices;
};

struct Node
{
  std::vector<double> position;
  std::vector<double> displacement;
  std::vector<double> force;
};

struct State
{
  std::int64_t step;
  double time;
  std::vector<double> displacement;
  std::vector<double> velocity;
  std::vector<double> acceleration;
  std::vector<double> force;
};

struct YamssException
{
  std::string what;
};

class handler
{
public:
  handler(const boost::filesystem::path& a_directory);

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
  ::yamss::transporter m_transporter;
  jobs_type m_jobs;
}; // handler class

} // yamss namespace

#endif // YAMSS_HANDLER_HPP
