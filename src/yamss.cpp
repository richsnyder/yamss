#include <complex>
#include "yamss/clp.hpp"
#include "yamss/xml_reader.hpp"

int
main(int argc, char* argv[])
{
  try
  {
    yamss::clp parser(argc, argv);
    if (!parser.good())
    {
      return 0;
    }

    if (parser.complex_mode())
    {
      typedef std::complex<double> value_type;
      typedef yamss::runner<value_type> runner_type;
      typedef boost::shared_ptr<runner_type> runner_pointer;
      runner_pointer runner = yamss::read_xml<value_type>(parser.input());
      runner->initialize();
      runner->run();
      runner->finalize();
    }
    else
    {
      typedef double value_type;
      typedef yamss::runner<value_type> runner_type;
      typedef boost::shared_ptr<runner_type> runner_pointer;
      runner_pointer runner = yamss::read_xml<value_type>(parser.input());
      runner->initialize();
      runner->run();
      runner->finalize();
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

  return 0;
}
