// new and delete of the whole core on mimalloc: every job, the simulation and the render thread have their own heap, where the C
// runtime's allocator has one lock for them all. Only new and delete: what Godot allocates stays in Godot, and nothing allocated
// here is freed by anybody outside this library (the C++ runtime is linked into it).
#include <mimalloc-new-delete.h>
