#include <iostream>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <climits>
#include <thread>
#include <chrono>
#include <map>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include "include/router.hpp"
#include "include/request.hpp"
#include "include/utils.hpp"

const int DBACKLOG = 16;
const uint32_t DPORT = 3955;

std::map<int, sockaddr> sockaddrs;
std::map<int, std::unique_ptr<std::thread>> threads;

std::unique_ptr<Router> router;
int msocketfd;

void handle(const int &cid, const int &connfd, const sockaddr_in &peeraddr) {
  char recvBuf[BUFFERSIZE];
  memset(recvBuf, 0, BUFFERSIZE);
  int bytestoread;
  for (int i = 0; i < 40; i ++, std::this_thread::sleep_for(std::chrono::milliseconds(50))) {
    if(ioctl(connfd, FIONREAD, &bytestoread) < 0) std::cerr << "ioctl error" << std::endl;
    if (bytestoread > 10) break;
  }
  int64_t msgsize = recv(connfd, recvBuf, BUFFERSIZE, MSG_DONTWAIT);
  if (msgsize == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      msgsize = 0;
    } else {
      std::cerr << "Connection [" << cid << "] recv error." << std::endl;
      close(connfd);
      return;
    }
  }
#ifdef DEBUG
  std::cout << "Recv from " << getHostWithPort(&peeraddr) << ", size = "
            << msgsize << std::endl << recvBuf << std::endl;
#endif
  // 解析请求
  HTTPRequest req;
  std::string reqStatus = req.parseRequest(recvBuf, msgsize);
  if (reqStatus == "Not HTTP") {
    close(connfd);
    std::cerr << "Connection [" << cid << "] closed normally." << std::endl;
    return;
  }
  if (reqStatus != "Ok") {
    std::cerr << "Connection [" << cid << "] parse request failed: " << reqStatus << std::endl;
    close(connfd);
    return;
  }
  HTTPResponse res;
  router->dispatch(req, res);
  size_t datalen = 0;
  const char *data = res.getData(datalen);
  int64_t flag = send(connfd, data, datalen, 0);
  if (flag == -1) {
    std::cerr << "Connection [" << cid << "] send error." << std::endl;
    close(connfd);
    return;
  }
  close(connfd);
  std::cerr << "Connection [" << cid << "] closed normally." << std::endl;
}

int main() {
  // create router
  router = std::make_unique<Router>();
  router->registerRule(HTTPMethod::mGET, "/", handleGET);
  router->registerRule(HTTPMethod::mPOST, "/dopost", handleDOPOST);

  // create socket
  msocketfd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in msockaddr;
  if (msocketfd == -1) {
    std::cerr << "Socket creation failed." << std::endl;
    return 1;
  }

  // bind socket
  msockaddr.sin_family = AF_INET;
  msockaddr.sin_port = htons(DPORT);
  msockaddr.sin_addr.s_addr = INADDR_ANY;
  memset(&(msockaddr.sin_zero), 0, 8);
  if ((bind(msocketfd, (sockaddr *)&msockaddr, sizeof(sockaddr))) == -1) {
    std::cerr << "Socket binding failed." << std::endl;
    return 1;
  }

  // listen on socket
  if (listen(msocketfd, DBACKLOG) == -1) {
    std::cerr << "Socket listening failed." << std::endl;
    return 1;
  }

  sockaddr_in mlistenaddr;
  socklen_t sin_size = sizeof(sockaddr_in);
  getsockname(msocketfd, (sockaddr *)&mlistenaddr, &sin_size); //获取监听的地址和端口
  std::cout << "Listening on port " << ntohs(mlistenaddr.sin_port) << "..." << std::endl;

  int threadcnt = 0;

  for(;;) {
    int mconnfd;
    sockaddr_in mpeeraddr;
    socklen_t sin_size = sizeof(sockaddr);
    if ((mconnfd = accept(msocketfd, (sockaddr *)&mpeeraddr, &sin_size)) == -1) {
      std::cerr << "Acceptance error." << std::endl;
      continue;
    }
    //使用accept接受新连接，如果失败则提示Acception Failed.
    std::cout << "Accepted connection [" << (++ threadcnt) << "] from " << getHostWithPort(&mpeeraddr) << std::endl;
    //提示成功连接上端口

    std::thread::id threadid;
    threads[threadcnt] = std::make_unique<std::thread>(std::thread(handle, threadcnt, mconnfd, mpeeraddr));
    threads[threadcnt]->detach();
    //创建线程，用threads[]维护
  }
  close(msocketfd);
  return 0;
}