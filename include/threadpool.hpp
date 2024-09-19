#pragma once

#include <map>
#include <functional>
#include <utility>

using taskFunc = void(*)(void *);

class Task {
  friend ThreadPool;
  public:
    Task(taskFunc f) : f(f) {}
  private:
    taskFunc f;
};

class ThreadPool {
  
};