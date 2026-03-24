#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>
template <typename T> class SafeQueue {
public:
  void push(const T &item) {
    std::lock_guard<std::mutex> lock(mutex);
    q.push(item);
    c.notify_one();
  }
  bool empty() {
    std::lock_guard<std::mutex> lock(mutex);
    return q.empty();
  }
  void pop(T &item) {
    std::unique_lock<std::mutex> lock(mutex);
    c.wait(lock, [this] { return !q.empty(); });

    item = q.front();
    q.pop();
  }

private:
  std::queue<T> q;
  std::mutex mutex;
  std::condition_variable c;
};

#endif
