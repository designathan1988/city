// A class declared in a header: a virtual method below a base, a field of a class type, a static maker.
#pragma once
#include "crate.h"

namespace fx {

class Counter {
 public:
  virtual int greet();
  int visit();
};

class Shop : public Counter {
 public:
  explicit Shop(int a);
  int open(int a);
  int greet() override;
  static Shop* make();

 private:
  Crate stock;
};

int helper(int n);

}  // namespace fx
