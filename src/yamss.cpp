#include "yamss/clp.hpp"
#include "yamss/xml_reader.hpp"

int
main(int argc, char* argv[])
{
  try
  {
    typedef double value_type;
    typedef yamss::clp parser_type;
    typedef yamss::runner<value_type> runner_type;
    typedef boost::shared_ptr<runner_type> runner_pointer;

    parser_type parser(argc, argv);
    if (!parser.good())
    {
      return 0;
    }

    runner_pointer runner = yamss::read_xml<value_type>(parser.input());
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
