// An operator applied to an object of a class calls the function its class writes for it.
#pragma once

struct Vec {
  float x = 0, y = 0;
  Vec operator-(const Vec &o) const { return Vec{x - o.x, y - o.y}; }
  float len() const { return x + y; }
};

struct Gauge {
  float len() const { return 0; }
};

inline float span(const Vec &a, const Vec &b) { return (a - b).len(); }
