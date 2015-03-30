#ifndef YAMSS_RUN_SIMULATION_HPP
#define YAMSS_RUN_SIMULATION_HPP

#include <boost/shared_ptr.hpp>
#include "yamss/clp.hpp"
#include "yamss/input_reader.hpp"
#include "yamss/runner.hpp"

namespace yamss {

template <typename T>
void
run_simulation(const clp& a_parser)
{
  boost::shared_ptr<runner<T> > runner_;
  if (a_parser.has_input_filename())
  {
    runner_ = read_input<T>(a_parser.input_filename());
  }
  else
  {
    runner_ = read_input<T>();
  }
  runner_->initialize();
  runner_->run();
  runner_->finalize();
}

} // yamss namespace

#endif // YAMSS_RUN_SIMULATION_HPP
