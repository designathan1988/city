// A template's method and a constructor with its initializers written out of their class; an object made with a
// variable as its argument; a function kept in a member and called through it; a name written with its scopes.
#include "kiln.h"

namespace fx::heat {

template <class T>
T Rack<T>::take() { return T(); }

Kiln::Kiln(fx::Crate& crate) : crate_(crate) { probe = &stoke; }

int Kiln::fire(int n) {
  fx::Crate spare;
  Kiln other(spare);
  other.fire(n);
  rack_.take().lift(n);
  return probe(n) + crate_.weigh(n) + fx::heat::fan(n);
}

int stoke(int n) { return n; }
int fan(int n) { return n; }

}  // namespace fx::heat
