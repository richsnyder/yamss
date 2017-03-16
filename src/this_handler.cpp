#include "yamss/this_handler.hpp"

namespace yamss {

std::map<std::thread::id, boost::shared_ptr<handler> > this_handler::c_handlers;
std::map<std::thread::id, std::string> this_handler::c_working_directories;

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
  boost::filesystem::path workdir = working_directory();
  workdir /= boost::filesystem::unique_path(model);
  boost::filesystem::create_directories(workdir);
  auto new_handler = boost::make_shared<handler>(workdir);
  c_handlers.insert(std::make_pair(id, new_handler));
  return new_handler;
}

std::string
this_handler::working_directory()
{
  auto id = std::this_thread::get_id();
  auto dir_ = c_working_directories.find(id);
  if (dir_ == c_working_directories.end())
  {
    return boost::filesystem::current_path().native();
  }
  else
  {
    return dir_->second;
  }
}

void
this_handler::working_directory(const std::string& a_path)
{
  auto id = std::this_thread::get_id();
  c_working_directories[id] = a_path;
}

} // yamss namespace
