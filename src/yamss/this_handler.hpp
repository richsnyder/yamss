#ifndef YAMSS_THIS_HANDLER_HPP
#define YAMSS_THIS_HANDLER_HPP

#include <thread>
#include <boost/make_shared.hpp>
#include "yamss/handler.hpp"

namespace yamss {

class this_handler
{
public:
  static boost::shared_ptr<handler> get();
private:
  static std::map<std::thread::id, boost::shared_ptr<handler> > c_handlers;
};

} // yamss namespace

#endif // YAMSS_THIS_HANDLER_HPP
