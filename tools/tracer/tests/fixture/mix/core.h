// A class below a base the code read does not define, one below none, and a function given an object nothing is
// known of.
#pragma once

class Core : public Object {
 public:
  int fire(int n) { return n; }
};

class Plain {
 public:
  int fire(int n) { return n; }
};

template <class T>
int hoist(T& any) { return any.lift(4) + any.fire(5); }
