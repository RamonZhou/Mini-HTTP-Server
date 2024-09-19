#pragma once

#include <queue>
#include <mutex>

template <class T>
class SafeQueue {
  public:
    void push(const T& x) {
      std::unique_lock<std::mutex> lock(_mutex);
      _q.push(x);
    }

    T top() const {
      std::unique_lock<std::mutex> lock(_mutex);
      return _q.top();
    }

    T pop() {
      std::unique_lock<std::mutex> lock(_mutex);
      T x = _q.top();
      _q.pop();
      return x;
    }

    bool empty() const {
      std::unique_lock<std::mutex> lock(_mutex);
      return _q.empty();
    }
  private:
    std::queue<T> _q;
    std::mutex _mutex;
};