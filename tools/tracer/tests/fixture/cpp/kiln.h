// A namespace written under another in one name, a class template, a variable holding a function, a field kept by
// reference.
#pragma once
#include "crate.h"

namespace fx::heat {

template <class T>
class Rack {
 public:
  T take();
};

class Kiln {
 public:
  explicit Kiln(fx::Crate& crate);
  int fire(int n);
  int (*probe)(int n);

 private:
  fx::Crate& crate_;
  Rack<fx::Crate> rack_;
};

int stoke(int n);
int fan(int n);

}  // namespace fx::heat
