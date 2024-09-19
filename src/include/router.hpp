#pragma once

#include <map>
#include <functional>
#include <utility>
#include "handlers.hpp"
#include "request.hpp"
#include "response.hpp"
#include "enums.h"

#define BUFFERSIZE 65535

using HTTPReqInfo = std::pair<std::string, HTTPMethod>;

class Router {
public:
  Router() {}
  
  void registerRule(const HTTPMethod &method, const std::string &url,
                    const std::function<void(const HTTPRequest &, HTTPResponse &)>&& func) {
    _rules[std::make_pair(url, method)] = func;
  }

  bool dispatch(const HTTPRequest &req, HTTPResponse &res) {
#ifdef DEBUG
    std::cerr << static_cast<int>(req.getMethod()) << " " << req.getUrl() << std::endl;
#endif
    if (req.getMethod() == HTTPMethod::mGET) {
      _rules[std::make_pair(std::string("/"), HTTPMethod::mGET)](req, res);
      return true;
    }
    if(_rules.find(std::make_pair(req.getUrl(), req.getMethod())) == _rules.end()) {
      badRequest(res);
      return false;
    }
    _rules[std::make_pair(req.getUrl(), req.getMethod())](req, res);
    return true;
  }
private:
  std::map<HTTPReqInfo, std::function<void(const HTTPRequest &, HTTPResponse &)>> _rules;
};