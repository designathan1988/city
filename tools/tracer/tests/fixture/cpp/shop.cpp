// The methods defined out of their class, a lambda kept in a local and called, a function handed by its address to a
// function the code read does not define, an object made with `new` and one made on the stack.
#include "shop.h"

namespace fx {

int Counter::greet() { return 0; }
int Counter::visit() { return greet(); }

Shop::Shop(int a) { stock.lift(a); }

int Shop::open(int a) {
  auto twice = [this](int x) { return stock.lift(x) + helper(x); };
  return twice(a);
}

int Shop::greet() { return helper(1); }

Shop* Shop::make() {
  Shop* made = new Shop(2);
  made->open(3);
  Shop local(4);
  local.visit();
  schedule(&helper);
  return made;
}

int helper(int n) { return n; }

}  // namespace fx
