// The other file's helper of the same name, and a function neither file declares where it is called.
namespace fx {

static int trim(int n) { return n + 1; }

int south(int n) { return trim(n); }

int polish(int n) { return n; }

}  // namespace fx
