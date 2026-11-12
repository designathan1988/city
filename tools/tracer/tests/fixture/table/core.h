// The core's functions, each handed over with the text of its name.
#pragma once

class Core : public Object {
 public:
  int seal_box() { return 1; }
  int wipe_box() { return 2; }
};

struct Binder {
  static void bind(const char* name, int (Core::*method)());
};

inline void bind_all() {
  Binder::bind(NAMED("seal_box"), &Core::seal_box);
  Binder::bind(NAMED("wipe_box"), &Core::wipe_box);
}
