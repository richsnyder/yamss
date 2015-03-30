#ifndef YAMSS_RUN_SERVER_HPP
#define YAMSS_RUN_SERVER_HPP

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <zenith/server.hpp>
#include "yamss/clp.hpp"
#include "yamss/server/handler.hpp"

namespace yamss {

void
run_server(const clp& a_parser);

} // yamss namespace

#endif // YAMSS_RUN_SERVER_HPP
