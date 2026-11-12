// A file's own helper, of the name another file gives its own: each file calls the one it can see.
#include "crate.h"

namespace fx {

namespace {
int trim(int n) { return n; }
}  // namespace

int north(int n) { return trim(n) + polish(n); }

int both(int n) {
  const int trim = n + 1, more = n + 2;
  schedule(trim);
  return more;
}

}  // namespace fx
