#include "yamss/run_server.hpp"

namespace yamss {

void
run_server(const yamss::clp& a_parser)
{
  using ::apache::thrift::TProcessor;
  using ::boost::shared_ptr;
  using ::yamss::server::handler;
  using ::yamss::server::YamssProcessor;
  using ::zenith::server;

  const std::string model = "yamss-%%%%-%%%%-%%%%-%%%%";
  boost::filesystem::path workdir = boost::filesystem::current_path();
  workdir /= boost::filesystem::unique_path(model);
  boost::filesystem::create_directory(workdir);

  const std::string endpoint = a_parser.server_endpoint();
  zctx_t* context = zctx_new();
  shared_ptr<handler> handler_(new handler(workdir));
  shared_ptr<TProcessor> processor(new YamssProcessor(handler_));
  shared_ptr<server> server_(new server(context, processor, endpoint));

  try
  {
    server_->serve();
  }
  catch (std::exception& e)
  {
    std::cout << std::endl << e.what() << std::endl;
  }

  zctx_destroy(&context);
  if (!a_parser.keep_files())
  {
    boost::filesystem::remove_all(workdir);
  }
}

} // yamss namespace
