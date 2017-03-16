#ifndef YAMSS_NATIVE_NATIVE_HPP
#define YAMSS_NATIVE_NATIVE_HPP

#include "yamss/this_handler.hpp"

namespace yamss {
namespace wrapper {

void
setWorkingDirectory(const std::string& a_path);

JobKey
create(const std::string& a_url) throw(YamssException);

void
release(const JobKey& a_job);

// Simulation control

void
initialize(const JobKey& a_job) throw(YamssException);

void
finalize(const JobKey& a_job) throw(YamssException);

void
step(const JobKey& a_job) throw(YamssException);

void
stepN(const JobKey& a_job, const std::int32_t a_steps) throw(YamssException);

void
advance(const JobKey& a_job) throw(YamssException);

void
subiterate(const JobKey& a_job) throw(YamssException);

void
report(const JobKey& a_job) throw(YamssException);

void
run(const JobKey& a_job) throw(YamssException);

void
runJob(const std::string& a_url) throw(YamssException);

void
setFinalTime(const JobKey& a_job, const double a_final_time) throw(YamssException);

// Queries

std::vector<bool>
getActiveDofs(const JobKey& a_job) throw(YamssException);

std::int32_t
getNumberOfActiveDofs(const JobKey& a_job) throw(YamssException);

std::int32_t
getNumberOfNodes(const JobKey& a_job) throw(YamssException);

double
getTime(const JobKey& a_job) throw(YamssException);

double
getTimeStep(const JobKey& a_job) throw(YamssException);

double
getFinalTime(const JobKey& a_job) throw(YamssException);

std::vector<double>
getModes(const JobKey& a_job) throw(YamssException);

Node
getNode(const JobKey& a_job, const std::int64_t a_nodeKey) throw(YamssException);

State
getState(const JobKey& a_job) throw(YamssException);

// Interface methods

InterfacePatch
getInterface(const JobKey& a_job,
             const std::int64_t a_loadKey) throw(YamssException);

InterfaceMovement
getInterfaceMovement(const JobKey& a_job,
                     const std::int64_t a_loadKey) throw(YamssException);

void
setInterfaceLoading(const JobKey& a_job,
                    const std::int64_t a_loadKey,
                    const InterfaceLoading& a_loading) throw(YamssException);

} // wrapper namespace
} // yamss namespace

#endif // YAMSS_NATIVE_NATIVE_HPP
