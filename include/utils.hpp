#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>

std::string getHostWithPort(const sockaddr_in *addr) {
  static char host[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &((*addr).sin_addr), host, sizeof(host));
  uint16_t port = ntohs((*addr).sin_port);
  return std::string(host) + ":" + std::to_string(port);
}