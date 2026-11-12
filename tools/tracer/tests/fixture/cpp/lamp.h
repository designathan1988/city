// A class below a base the code read does not define; functions given an object of a type it does not define, of
// that base, of a template's parameter and of an alias.
#pragma once
#include <node.h>
#include "crate.h"

namespace fx {

class Lamp : public Node {
 public:
  int lift(int a) { return a; }
};

using Box = fx::Crate;

inline int drift(Vec v) { return v.lift(1); }
inline int raise(Node* node) { return node->lift(2); }
template <class T>
int hoist(T& any) { return any.lift(3); }
inline int boxed(Box& box) { return box.lift(4); }
inline int recast(void* raw) { return static_cast<Crate*>(raw)->lift(5) + ((Crate*)raw)->weigh(6); }
inline void choose(bool up) { schedule(up ? &drift : (&raise)); }

struct Slot {
  int (*run)(Vec v);
};
inline void fill(Slot& slot) { slot.run = &drift; }
inline void peek(Vec v) { schedule(v.run); }
template <class T>
int poke(T& any, Vec v) { return any.run(v); }

}  // namespace fx
