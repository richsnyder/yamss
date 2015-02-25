#include "yamss/clp.hpp"

namespace yamss {

clp::clp(int argc, char* argv[])
  : m_program_name(boost::filesystem::path(argv[0]).filename().c_str())
  , m_argument_options("Arguments")
  , m_visible_options("Options")
  , m_positional()
  , m_variables_map()
{
  initialize();
  parse(argc, argv);
  report(std::cout);
}

bool
clp::good() const
{
  return
      m_variables_map.count("help") == 0 &&
      m_variables_map.count("legal") == 0 &&
      m_variables_map.count("version") == 0 &&
      m_variables_map.count("input") == 1;
}

void
clp::initialize()
{
  namespace po = boost::program_options;
  m_argument_options.add_options()
    (
      "input",
      po::value<std::string>(),
      "input XML file"
    );
  m_visible_options.add_options()
    (
      "complex,c",
      "enable complex mode"
    )(
      "help,h",
      "print this help message and exit"
    )(
      "legal,l",
      "print a legal statement and exit"
    )(
      "version,v",
      "print the version and exit"
    );
  m_positional.add("input", 1);
}

bool
clp::complex_mode() const
{
  return m_variables_map.count("complex") == 1;
}

std::string
clp::input() const
{
  return m_variables_map["input"].as<std::string>();
}

void
clp::legal(std::ostream& a_out) const
{
  a_out << about::legal() << std::endl;
}

void
clp::parse(int argc, char* argv[])
{
  namespace po = boost::program_options;
  po::options_description all("All options");
  all.add(m_argument_options).add(m_visible_options);
  po::command_line_parser parser(argc, argv);
  parser.options(all);
  parser.positional(m_positional);
  po::store(parser.run(), m_variables_map);
  po::notify(m_variables_map);
}

void
clp::report(std::ostream& a_out) const
{
  bool has_help = m_variables_map.count("help");
  bool has_input = m_variables_map.count("input");
  bool has_legal = m_variables_map.count("legal");
  bool has_version = m_variables_map.count("version");

  if (has_help && has_legal)
  {
    legal(a_out);
    a_out << std::endl;
    usage(a_out);
  }
  else if (has_help && has_version)
  {
    version(a_out);
    a_out << std::endl;
    usage(a_out);
  }
  else if (has_help)
  {
    usage(a_out);
  }
  else if (has_legal)
  {
    legal(a_out);
  }
  else if (has_version)
  {
    version(a_out);
  }
  else if (!has_input)
  {
    usage(a_out);
  }
}

void
clp::usage(std::ostream& a_out) const
{
  a_out << "Usage: " << m_program_name << " [options] input" << std::endl;
  a_out << std::endl;
  a_out << m_visible_options;
}

void
clp::version(std::ostream& a_out) const
{
  a_out << about::description() << std::endl;
}

} // yamss namespace
