#pragma once

#include <string>
#include <cstring>
#include <cstdlib>
#include "enums.h"

#define BUFFERSIZE 65535

class HTTPResponse {
public:
  void setStatusCode(const HTTPStatusCode code) {
    _code = code;
  }
  void setHeader(const std::string &key, const std::string &value) {
    _headers[key] = value;
  }
  void appendContent(const std::string data) {
    _content += data;
  }
  const char *getData(size_t &size) {
    _buf = "HTTP/1.1 " + std::to_string(static_cast<int>(_code)) + "\r\n";
    _buf += "Server: myHTTPServer\r\n";
    _buf += "Cache-Control: no-cache\r\n";
    for (auto p: _headers) {
      _buf += p.first + ": " + p.second + "\r\n";
    }
    _buf += "\r\n";
    _buf += _content;
    size = _buf.size();
#ifdef DEBUG
    std::cerr << "Going to send: " << std::endl << _buf << std::endl;
#endif
    return _buf.c_str();
  }
private:
  HTTPStatusCode _code;
  std::unordered_map<std::string, std::string> _headers;
  std::string _content;
  std::string _buf;
};