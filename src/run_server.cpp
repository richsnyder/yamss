#include "yamss/server/run_server.hpp"

namespace yamss {
namespace server {

void
run_server(const yamss::clp& a_parser)
{
  using ::boost::shared_ptr;
  using ::yamss::server::server;

  const std::string model = "yamss-%%%%-%%%%-%%%%-%%%%";
  boost::filesystem::path workdir = boost::filesystem::current_path();
  workdir /= boost::filesystem::unique_path(model);
  boost::filesystem::create_directory(workdir);

  void* context = zmq_ctx_new();
  const std::string endpoint = a_parser.server_endpoint();
  server server_(context, endpoint, ZMQ_REP, workdir);

  try
  {
    server_.serve();
  }
  catch (std::exception& e)
  {
    std::cout << std::endl << e.what() << std::endl;
  }

  zmq_ctx_term(context);
  if (!a_parser.keep_files())
  {
    boost::filesystem::remove_all(workdir);
  }
}

} // server namespace
} // yamss namespace
