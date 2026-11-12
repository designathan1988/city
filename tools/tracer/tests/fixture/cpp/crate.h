// A class whose methods are written in its body.
#pragma once

namespace fx {

class Crate {
 public:
  int lift(int a) { return weigh(a); }
  int weigh(int a) { return a; }
};

}  // namespace fx
