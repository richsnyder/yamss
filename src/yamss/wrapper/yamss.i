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

namespace wrapper {

// Job control

JobKey create(const std::string& a_url) throw(YamssException);
void release(const JobKey& a_job);

// Simulation control

void initialize(const JobKey& a_job) throw(YamssException);
void finalize(const JobKey& a_job) throw(YamssException);
void step(const JobKey& a_job) throw(YamssException);
void stepN(const JobKey& a_job, const std::int32_t a_steps) throw(YamssException);
void advance(const JobKey& a_job) throw(YamssException);
void subiterate(const JobKey& a_job) throw(YamssException);
void report(const JobKey& a_job) throw(YamssException);
void run(const JobKey& a_job) throw(YamssException);
void runJob(const std::string& a_url) throw(YamssException);
void setFinalTime(const JobKey& a_job, const double a_final_time) throw(YamssException);

// Queries

std::vector<bool> getActiveDofs(const JobKey& a_job) throw(YamssException);
std::int32_t getNumberOfActiveDofs(const JobKey& a_job) throw(YamssException);
std::int32_t getNumberOfNodes(const JobKey& a_job) throw(YamssException);
double getTime(const JobKey& a_job) throw(YamssException);
double getTimeStep(const JobKey& a_job) throw(YamssException);
double getFinalTime(const JobKey& a_job) throw(YamssException);
std::vector<double> getModes(const JobKey& a_job) throw(YamssException);
Node getNode(const JobKey& a_job, const std::int64_t a_nodeKey) throw(YamssException);
State getState(const JobKey& a_job) throw(YamssException);

// Interface methods

InterfacePatch
getInterface(const JobKey& a_job,
             const std::int64_t a_loadKey) throw(YamssException);

InterfaceMovement
getMovement(const JobKey& a_job,
            const std::int64_t a_loadKey) throw(YamssException);

void
setLoading(const JobKey& a_job,
           const std::int64_t a_loadKey,
           const InterfaceLoading& a_loading) throw(YamssException);

}
}

namespace std {

%template(BooleanVector) vector<bool>;
%template(DoubleVector) vector<double>;
%template(Int32Vector) vector<std::int32_t>;
%template(ElementTypeVector) vector<yamss::ElementType>;

}
