#ifndef YAMSS_THIS_HANDLER_HPP
#define YAMSS_THIS_HANDLER_HPP

#include <string>
#include <thread>
#include <boost/make_shared.hpp>
#include "yamss/handler.hpp"

namespace yamss {

class this_handler
{
public:
  static boost::shared_ptr<handler> get();
  static std::string working_directory();
  static void working_directory(const std::string& a_path);
private:
  static std::map<std::thread::id, boost::shared_ptr<handler> > c_handlers;
  static std::map<std::thread::id, std::string> c_working_directories;
};

} // yamss namespace

#endif // YAMSS_THIS_HANDLER_HPP
