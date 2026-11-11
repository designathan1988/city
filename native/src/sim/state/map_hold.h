// Who holds the map and what is done under it (CityState::map_mutex). Shared: what a thread builds while it holds the map
// (taken through MapRead: city_state.h); a mesh job copies what it reads under the lock and builds with the lock let go
// (sim/state/city_cut.h), and a vertex or an instance a builder takes while its thread holds the map is counted here, in the tests'
// DLL (core/checks.h), by the lane of the job that took it, so the map_hold probe says which producer still builds under the lock.
// Exclusive: every hold names its writer (WriterHold): a command of the player, the simulation's tick, a city opened or
// generated, the save marking what its file holds, a probe of the tests setting what it measures. A change to the map
// (CityData::touch) made on a thread with no writer named stops the tests' DLL, and the probe counts the holds by writer.
#pragma once

#include "core/checks.h"

#include <cstdint>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>

namespace city {
namespace map_hold {

enum class Writer : uint8_t { NONE, COMMAND, TICK, LOAD, GENERATION, SAVE, PROBE };
const char *writer_name(Writer w);

inline Writer &writing() {
	thread_local Writer w = Writer::NONE;
	return w;
}
void held_by(Writer w); // an exclusive hold counted by its writer (the tests' DLL)
std::map<std::string, int64_t> writers(); // the exclusive holds since the last clear, by writer
inline void must_write() {
	if (CHECKS && writing() == Writer::NONE) city_stop("the map changed on a thread with no writer named (sim/state/map_hold.h WriterHold)");
}

// A writer named for a scope without the map's lock: only for a city no other thread sees (a file read aside, load_city).
class Named {
public:
	explicit Named(Writer who) : prev_(writing()) { writing() = who; }
	~Named() { writing() = prev_; }
	Named(const Named &) = delete;
	Named &operator=(const Named &) = delete;

private:
	Writer prev_;
};

// The map held exclusively by a named writer, the one way to hold it so (tools/gates/rules.json MAP_WRITER_UNNAMED).
class WriterHold {
public:
	WriterHold(std::shared_mutex &m, Writer who) : lock_(m), prev_(writing()) { name(who); }
	WriterHold(std::shared_mutex &m, Writer who, std::try_to_lock_t t) : lock_(m, t), prev_(writing()) {
		if (lock_.owns_lock()) name(who);
	}
	~WriterHold() { writing() = prev_; } // the writer goes back before the lock is let go (members end after the body)
	WriterHold(const WriterHold &) = delete;
	WriterHold &operator=(const WriterHold &) = delete;
	bool owns_lock() const { return lock_.owns_lock(); }

private:
	void name(Writer who) {
		writing() = who;
		if (CHECKS) held_by(who);
	}
	std::unique_lock<std::shared_mutex> lock_;
	Writer prev_;
};

struct Held {
	int depth = 0; // MapRead guards open on this thread
	int64_t built = 0; // what it built under them, not yet handed to the tally
};
inline Held &mine() {
	thread_local Held held;
	return held;
}

inline void enter() { mine().depth++; }
void leave(); // the last guard of the thread hands its count to the tally
// One vertex or one instance taken by a builder: counted when the calling thread holds the map.
inline void built() {
	if (!CHECKS) return;
	Held &h = mine();
	if (h.depth > 0) h.built++;
}

// What was built under the map since the last clear: by the lane of the keyed job that built it (core/job_pool.h job_lane_name),
// "outside" for a thread in no keyed job (the frame's thread).
std::map<std::string, int64_t> tally();
void clear(); // both tallies: what was built under the map and the exclusive holds by writer

} // namespace map_hold
} // namespace city
