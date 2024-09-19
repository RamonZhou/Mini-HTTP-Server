#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "request.hpp"
#include "response.hpp"
#include "enums.h"

#define BUFFERSIZE 65535

void badRequest(HTTPResponse &res) {
  res.setStatusCode(HTTPStatusCode::mBadRequest);
  res.setHeader("Content-Length", "0");
}

void handleGET(const HTTPRequest &req, HTTPResponse &res){
  std::string fileurl = req.getUrl();
  std::ifstream fin(std::string("/mnt/d/courses/network/lab3/test") + fileurl, std::ios::binary);
  if (!fin) {
    fileurl = req.getUrl() + ".html";
    fin.open(std::string("/mnt/d/courses/network/lab3/test") + fileurl, std::ios::binary);
  }
  if (!fin) {
    fileurl = req.getUrl() + "/index.html";
    fin.open(std::string("/mnt/d/courses/network/lab3/test") + fileurl, std::ios::binary);
  }
  if (!fin) {
    res.setStatusCode(HTTPStatusCode::mNotFound);
    res.setHeader("Content-Type", "application/json");
    res.setHeader("Content-Length", "0");
    return;
  }
  res.setStatusCode(HTTPStatusCode::mOk);
  size_t splitpos = fileurl.find_last_of(".");
  std::string format = splitpos == std::string::npos ? "no" : fileurl.substr(splitpos + 1);
  if (format == "html") res.setHeader("Content-Type", "text/html;charset=utf-8");
  else if (format == "jpg" || format == "jpeg") res.setHeader("Content-Type", "image/jpeg");
  else if (format == "txt") res.setHeader("Content-Type", "text/plain");
  else res.setHeader("Content-Type", "application/octet-stream");
  char *buffer = new char[BUFFERSIZE];
  fin.seekg(0, std::ios::end);
  std::streampos endpos = fin.tellg();
  res.setHeader("Content-Length", std::to_string(endpos));
  fin.seekg(0, std::ios::beg);
  fin.read(buffer, endpos);
  fin.close();
  res.appendContent(std::string(buffer, endpos));
  delete buffer;
}

void handleDOPOST(const HTTPRequest &req, HTTPResponse &res){
  std::string login = req.getBodyVal("login");
  std::string pass = req.getBodyVal("pass");
  res.setStatusCode(HTTPStatusCode::mOk);
  res.setHeader("Content-Type", "text/plain;charset=utf-8");
  if (login.length() == 10 && login.substr(login.length() - 4) == pass) {
    res.setHeader("Content-Length", std::to_string(strlen("登录成功")));
    res.appendContent("登录成功");
  } else {
    res.setHeader("Content-Length", std::to_string(strlen("登录失败")));
    res.appendContent("登录失败");
  }
}