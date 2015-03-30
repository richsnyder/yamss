#ifndef YAMSS_SERVER_HANDLER_HPP
#define YAMSS_SERVER_HANDLER_HPP

#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include "yamss/input_reader.hpp"
#include "yamss/runner.hpp"
#include "yamss/server/transporter.hpp"
#include "yamss/server/Yamss.h"

namespace yamss {
namespace server {

class handler : virtual public YamssIf
{
public:
  typedef boost::filesystem::path path_type;

  handler(const path_type& a_working_directory);

  void
  run(const std::string& a_url);
private:
  transporter m_transporter;
  path_type m_working_directory;
}; // handler class

} // server namespace
} // yamss namespace

#endif // YAMSS_SERVER_HANDLER_HPP
