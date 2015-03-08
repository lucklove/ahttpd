// ConnectionManager.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
//

#include "ConnectionManager.hh"

namespace http {
namespace server {

ConnectionManager::ConnectionManager()
{
}

void ConnectionManager::start(connection_ptr c)
{
  connections_.insert(c);
  c->start();
}

void ConnectionManager::stop(connection_ptr c)
{
  connections_.erase(c);
  c->stop();
}

void ConnectionManager::stop_all()
{
  for (auto c: connections_)
    c->stop();
  connections_.clear();
}

} // namespace server
} // namespace http
