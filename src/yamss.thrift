namespace cpp yamss.server

exception YamssException {
  1: string what
}

struct Interface {
  1: i64 key,
  2: list<bool> activeDofs,
  3: map<i64, list<double>> positions
}

struct Node {
  1: list<double> position,
  2: list<double> displacement,
  3: list<double> force
}

struct State {
  1: double time,
  2: list<double> displacement,
  3: list<double> velocity,
  4: list<double> acceleration,
  5: list<double> force
}

service Yamss {

  void
  finalize(1: string a_job_key)
           throws (1: YamssException e),

  list<Interface>
  getInterfaces(1: string a_job_key)
                throws (1: YamssException e),

  list<double>
  getModes(1: string a_job_key)
           throws (1: YamssException e),

  Node
  getNode(1: string a_job_key,
          2: i64 a_node_key)
          throws (1: YamssException e),

  State
  getState(1: string a_job_key)
           throws (1: YamssException e),

  double
  getTime(1: string a_job_key)
          throws (1: YamssException e),

  double
  getTimeStep(1: string a_job_key)
              throws (1: YamssException e),

  string
  initialize(1: string a_url)
             throws (1: YamssException e),

  void
  run(1: string a_job_key)
      throws (1: YamssException e),

  void
  runJob(1: string a_url)
         throws (1: YamssException e),

  void
  step(1: string a_job_key)
       throws (1: YamssException e),

}
