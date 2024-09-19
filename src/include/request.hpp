#pragma once

#include <iostream>
#include <sstream>
#include <unordered_map>
#include "enums.h"

#define BUFFERSIZE 65535

class HTTPRequest {
public:
  HTTPRequest() {}
  HTTPRequest(const char* buf, const size_t &bufsize) {
    parseRequest(buf, bufsize);
  }
  std::string parseRequest(const char* buf, const int64_t &bufsize) {
    // 先判断是不是 http
    bool ishttp = false;
    for (int i = 0; i < bufsize - 3; ++ i) {
      if (buf[i] == '\r' && buf[i + 1] == '\n' &&
          buf[i + 2] == '\r' && buf[i + 3] == '\n') {
        ishttp = true;
        break;
      }
    }
    if (!ishttp) return std::string("Not HTTP");

    char linebuf[BUFFERSIZE];
    std::stringstream ss;
    ss << std::string(buf, bufsize);
    ss.getline(linebuf, BUFFERSIZE);
    std::stringstream parserss(linebuf);
    std::string method, encodedurl, version;
    parserss >> method >> encodedurl >> version;

    if (version != "HTTP/1.1") {
      return std::string("Unsupported http version");
    }

    // 方法
    if (method == "GET") {
      _method = HTTPMethod::mGET;
    } else if (method == "POST") {
      _method = HTTPMethod::mPOST;
    } else if (method == "PUT") {
      _method = HTTPMethod::mPUT;
    } else if (method == "DELETE") {
      _method = HTTPMethod::mDELETE;
    } else {
      _method = HTTPMethod::mUnknown;
    }
    if (_method != HTTPMethod::mGET && _method != HTTPMethod::mPOST) {
      return std::string("Unsupported method " + method);
    }

    // url 和参数
    size_t splitpos = encodedurl.find_first_of("?#");
    if (splitpos == std::string::npos) _url = encodedurl;
    else {
      _url = encodedurl.substr(0, splitpos);
      std::string paramstr;
      if (encodedurl[splitpos] == '?') {
        do {
          size_t prepos = splitpos;
          splitpos = encodedurl.find_first_of("&#", splitpos + 1);
          if (splitpos == std::string::npos) splitpos = encodedurl.length();
          paramstr = encodedurl.substr(prepos + 1, (splitpos - prepos - 1));
          size_t spl = paramstr.find_first_of("=");
          _params[paramstr.substr(0, spl)] = paramstr.substr(spl + 1);
        } while (splitpos != encodedurl.length() && encodedurl[splitpos] != '#');
      }
    }

    if (_url[_url.length() - 1] == '/') _url.pop_back();

    // http headers
    for(ss.getline(linebuf, BUFFERSIZE);;ss.getline(linebuf, BUFFERSIZE)) {
      if (linebuf[0] == '\r') break;
      std::string headerstr(linebuf);
      size_t splitpos = headerstr.find_first_of(":");
      _headers[headerstr.substr(0, splitpos)] = headerstr.substr(splitpos + 2);
    }

#ifdef DEBUG
    std::cerr << "Method: " << method << "; URL: " << _url << "; Params: " << _params.size() 
              << "; Headers: " << _headers.size() << std::endl;
#endif

    // post body
    std::string content = "";
    int contentlen = _headers["Content-Length"] == "" ? 0 : std::stoi(_headers["Content-Length"]);
    if (contentlen == 0) return "Ok";
    while(ss.getline(linebuf, BUFFERSIZE)) {
      content += linebuf;
    }
    if (_headers["Content-Type"].find("application/x-www-form-urlencoded") != std::string::npos) {
      std::string paramstr = "";
      do {
        size_t prepos = splitpos;
        splitpos = content.find_first_of("&", splitpos + 1);
        if (splitpos == std::string::npos) splitpos = content.length();
        paramstr = content.substr(prepos + 1, (splitpos - prepos - 1));
        size_t spl = paramstr.find_first_of("=");
        _body[paramstr.substr(0, spl)] = paramstr.substr(spl + 1);
      } while (splitpos != content.length() && content[splitpos] != '#');
    } else if (_headers["Content-Type"].find("application/json") != std::string::npos) {
      // TO-DO
      if (_method != HTTPMethod::mGET) return "Unsupported Content-Type: Neglected";
      else return "Ok";
    } else {
      if (_method != HTTPMethod::mGET) return "Unsupported Content-Type: Neglected";
      else return "Ok";
    }

    return "Ok";
  }

  HTTPMethod getMethod() const {
    return _method;
  }

  std::string getUrl() const {
    return _url;
  }

  std::string getHeaderVal(const std::string &key) const {
    return _headers.find(key)->second;
  }

  std::string getParamVal(const std::string &key) const {
    return _params.find(key)->second;
  }

  std::string getBodyVal(const std::string &key) const {
    return _body.find(key)->second;
  }

  std::unordered_map<std::string, std::string>* getBodyMapPtr() {
    return &_body;
  }
  
private:
  HTTPMethod _method;
  std::string _url;
  std::unordered_map<std::string, std::string> _headers;
  std::unordered_map<std::string, std::string> _params;
  std::unordered_map<std::string, std::string> _body;
};