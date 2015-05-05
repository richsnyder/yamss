#include <complex>
#include "yamss/clp.hpp"
#ifdef YAMSS_SUPPORTS_SERVER_MODE
# include "yamss/run_server.hpp"
#endif
#include "yamss/run_simulation.hpp"

int
main(int argc, char* argv[])
{
  try
  {
    yamss::clp parser(argc, argv, "tcp://*:49200");
    if (!parser.good())
    {
      return 0;
    }

#ifdef YAMSS_SUPPORTS_SERVER_MODE

    if (parser.server_mode())
    {
      yamss::run_server(parser);
    }
    else if (parser.complex_mode())
    {
      typedef std::complex<double> value_type;
      yamss::run_simulation<value_type>(parser);
    }
    else
    {
      typedef double value_type;
      yamss::run_simulation<value_type>(parser);
    }

#else

    if (parser.complex_mode())
    {
      typedef std::complex<double> value_type;
      yamss::run_simulation<value_type>(parser);
    }
    else
    {
      typedef double value_type;
      yamss::run_simulation<value_type>(parser);
    }

#endif

  }
  catch (std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

  return 0;
}
