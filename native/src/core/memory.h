// The core's allocator as its threads use it (core/memory.cpp: new and delete on mimalloc, a heap a thread): a thread about to
// sleep gives back to the system what its work freed.
#pragma once

namespace city {

// How long freed memory waits before it may be given back (the allocator's own purge delay), in milliseconds: a thread idle for
// longer than this has nothing more to gain from keeping it.
long freed_memory_wait_ms();
// Gives the calling thread's freed memory back to the system. The allocator does this only when the thread allocates or frees
// again (its purge runs on the heap's own activity, never from another thread), so a worker that built meshes and went to sleep
// kept every page its jobs had used: the blocks are freed by the frame's thread, after the upload.
void release_freed_memory();

} // namespace city
