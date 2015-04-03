namespace cpp yamss.server

exception YamssException {
  1: string what
}

typedef string JobKey

enum ElementType {
  POINT,
  LINE,
  TRIANGLE,
  QUADRILATERAL
}

struct Interface {
  1: list<bool> activeDofs,
  2: list<double> nodeCoordinates,
  3: list<ElementType> elementTypes,
  4: list<i32> elementVertices
}

struct InterfaceLoading {
  1: list<double> forces
}

struct InterfaceMovement {
  1: list<double> displacements,
  2: list<double> velocities,
  3: list<double> accelerations
}

struct Node {
  1: list<double> position,
  2: list<double> displacement,
  3: list<double> force
}

struct State {
  1: i64 step,
  2: double time,
  3: list<double> displacement,
  4: list<double> velocity,
  5: list<double> acceleration,
  6: list<double> force
}

service Yamss {

  // Job management

  JobKey create(1: string a_url) throws (1: YamssException e),

  void release(1: JobKey a_key),

  // Simulation control

  void initialize(1: JobKey a_key) throws (1: YamssException e),

  void finalize(1: JobKey a_key) throws (1: YamssException e),

  void step(1: JobKey a_key) throws (1: YamssException e),

  void run(1: JobKey a_key) throws (1: YamssException e),

  void runJob(1: string a_url) throws (1: YamssException e),

  void setFinalTime(1: JobKey a_key, 2: double a_final_time)
            throws (1: YamssException e),

  // Queries

  double getTime(1: JobKey a_key) throws (1: YamssException e),

  double getTimeStep(1: JobKey a_key) throws (1: YamssException e),

  double getFinalTime(1: JobKey a_key) throws (1: YamssException e),

  list<double> getModes(1: JobKey a_key) throws (1: YamssException e),

  Node getNode(1: JobKey a_key, 2: i64 a_node_key) throws (1: YamssException e),

  State getState(1: JobKey a_key) throws (1: YamssException e),

  // Interface methods

  Interface getInterface(1: JobKey a_key, 2: i64 a_load_key)
                 throws (1: YamssException e),

  InterfaceMovement getInterfaceMovement(1: JobKey a_key, 2: i64 a_load_key)
                                 throws (1: YamssException e),

  void setInterfaceLoading(1: JobKey a_key,
                           2: i64 a_load_key,
                           3: InterfaceLoading a_loading)
                   throws (1: YamssException e),

}
