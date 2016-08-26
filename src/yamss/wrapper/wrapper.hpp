#ifndef YAMSS_NATIVE_NATIVE_HPP
#define YAMSS_NATIVE_NATIVE_HPP

#include "yamss/this_handler.hpp"

namespace yamss {
namespace wrapper {

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
getInterface(const JobKey& a_job,
             const std::int64_t a_loadKey);

InterfaceMovement
getInterfaceMovement(const JobKey& a_job,
                     const std::int64_t a_loadKey);

void
setInterfaceLoading(const JobKey& a_job,
                    const std::int64_t a_loadKey,
                    const InterfaceLoading& a_loading);

} // wrapper namespace
} // yamss namespace

#endif // YAMSS_NATIVE_NATIVE_HPP
