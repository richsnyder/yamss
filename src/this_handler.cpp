#include "yamss/this_handler.hpp"

namespace yamss {

std::map<std::thread::id, boost::shared_ptr<handler> > this_handler::c_handlers;

boost::shared_ptr<handler>
this_handler::get()
{
  auto id = std::this_thread::get_id();
  auto handler_ = c_handlers.find(id);
  if (handler_ != c_handlers.end())
  {
    return handler_->second;
  }

  const std::string model = "yamss-%%%%-%%%%-%%%%-%%%%";
  boost::filesystem::path workdir = boost::filesystem::current_path();
  workdir /= boost::filesystem::unique_path(model);
  boost::filesystem::create_directory(workdir);
  auto new_handler = boost::make_shared<handler>(workdir);
  c_handlers.insert(std::make_pair(id, new_handler));
  return new_handler;
}

} // yamss namespace
