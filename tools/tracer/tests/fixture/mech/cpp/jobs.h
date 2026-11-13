// A queue of functions kept in aggregate items and run later; state written by one member, read by another.
#pragma once
#include <functional>
#include <queue>

struct Job { int rank; std::function<void()> work; };

class Pool {
 public:
  void submit(std::function<void()> work) { q_.push(Job{ 1, std::move(work) }); }
  void loop() { Job j = q_.front(); auto w = std::move(j.work); w(); }
 private:
  std::queue<Job> q_;
};

inline void bake() {}
inline void start(Pool &p) { p.submit([] { bake(); }); }

struct Tally { int count = 0; };

class Ledger {
 public:
  Tally t;
  void bump() { t.count++; }
  int read() { return t.count; }
};
