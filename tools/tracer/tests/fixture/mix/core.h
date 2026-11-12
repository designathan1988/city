// A class below a base the code read does not define, one below none, and a function given an object nothing is
// known of.
#pragma once

class Core : public Object {
 public:
  int fire(int n) { return n; }
  int sweep(int n) { return n; }
};

struct Binder {
  static void bind(const char* name, int (Core::*method)(int));
};

inline void bind_all() {
  Binder::bind(NAMED("fire", "n"), &Core::fire);
  Binder::bind(NAMED("sweep", "n"), &Core::sweep);
}

class Plain {
 public:
  int fire(int n) { return n; }
};

template <class T>
int hoist(T& any) { return any.lift(4) + any.fire(5); }
