%module yamss
%{
#include "yamss/wrapper/wrapper.hpp"
%}

%include "stdint.i"
%include "std_string.i"
%include "std_vector.i"

namespace yamss {

enum ElementType { POINT, LINE, TRIANGLE, QUADRILATERAL };
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
  std::vector<bool> activeDofs;
  std::vector<double> nodeCoordinates;
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

namespace wrapper {

// Job control

JobKey create(const std::string& a_url);
void release(const JobKey& a_job);

// Simulation control

void initialize(const JobKey& a_job);
void finalize(const JobKey& a_job);
void step(const JobKey& a_job);
void stepN(const JobKey& a_job, const std::int32_t a_steps);
void advance(const JobKey& a_job);
void subiterate(const JobKey& a_job);
void report(const JobKey& a_job);
void run(const JobKey& a_job);
void runJob(const std::string& a_url);
void setFinalTime(const JobKey& a_job, const double a_final_time);

// Queries

std::vector<bool> getActiveDofs(const JobKey& a_job);
std::int32_t getNumberOfActiveDofs(const JobKey& a_job);
std::int32_t getNumberOfNodes(const JobKey& a_job);
double getTime(const JobKey& a_job);
double getTimeStep(const JobKey& a_job);
double getFinalTime(const JobKey& a_job);
std::vector<double> getModes(const JobKey& a_job);
Node getNode(const JobKey& a_job, const std::int64_t a_nodeKey);
State getState(const JobKey& a_job);

// Interface methods

InterfacePatch getInterface(const JobKey& a_job, const std::int64_t a_loadKey);
InterfaceMovement getInterfaceMovement(const JobKey& a_job, const std::int64_t a_loadKey);
void setInterfaceLoading(const JobKey& a_job, const std::int64_t a_loadKey, const InterfaceLoading& a_loading);

}
}

namespace std {

%template(BooleanVector) vector<bool>;
%template(DoubleVector) vector<double>;
%template(Int32Vector) vector<std::int32_t>;
%template(ElementTypeVector) vector<yamss::ElementType>;

}
