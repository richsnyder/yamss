#include "yamss/xml_reader.hpp"

int
main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "ERROR: One argument is required" << std::endl;
    return 0;
  }

  try
  {
    typedef double value_type;
    typedef yamss::runner<value_type> runner_type;
    typedef boost::shared_ptr<runner_type> runner_pointer;

    runner_pointer runner = yamss::read_xml<value_type>(argv[1]);
    runner->initialize();
    runner->run();
    runner->finalize();
  }
  catch (std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

  return 0;
}
