#ifndef YAMSS_SERVER_RUN_SERVER_HPP
#define YAMSS_SERVER_RUN_SERVER_HPP

#include <boost/filesystem.hpp>
#include "yamss/clp.hpp"
#include "yamss/server/server.hpp"

namespace yamss {
namespace server {

void
run_server(const clp& a_parser);

} // server namespace
} // yamss namespace

#endif // YAMSS_SERVER_RUN_SERVER_HPP
