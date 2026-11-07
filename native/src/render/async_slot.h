// One background job of a field (fog, canyon, shore) and its result, under a generation: reset moves the generation and lets
// go of the result, the busy mark and the job's scratch, so a job of an older generation still running publishes nothing and a
// new one can start at once. The scratch (buffers a job reuses, a cache, the previous raster) passes from the slot to the job and
// back, so two jobs never share it.
#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <utility>

namespace city {

template <typename Raster, typename Scratch>
class AsyncSlot {
public:
	// Main thread: no job of this generation is running or waiting to be taken.
	bool busy() const {
		std::lock_guard<std::mutex> lk(mutex_);
		return busy_;
	}
	bool ready() const {
		std::lock_guard<std::mutex> lk(mutex_);
		return has_done_;
	}
	// The job: no reset came since it started (it stops at its next natural boundary otherwise: nothing it makes is kept).
	bool current(uint64_t generation) const {
		std::lock_guard<std::mutex> lk(mutex_);
		return generation == generation_;
	}
	// Main thread: a job starts; it carries the generation it belongs to.
	uint64_t begin() {
		std::lock_guard<std::mutex> lk(mutex_);
		busy_ = true;
		return generation_;
	}
	// The job: the scratch the last job of this generation left (fresh when there is none).
	std::unique_ptr<Scratch> scratch(uint64_t generation) {
		std::lock_guard<std::mutex> lk(mutex_);
		if (generation != generation_ || !spare_) return std::make_unique<Scratch>();
		return std::move(spare_);
	}
	// The job's end: its result and scratch kept only if no reset came meanwhile.
	void publish(uint64_t generation, Raster &&r, std::unique_ptr<Scratch> scratch) {
		std::unique_ptr<Scratch> gone;
		{
			std::lock_guard<std::mutex> lk(mutex_);
			if (generation != generation_) {
				gone = std::move(scratch);
			} else {
				done_ = std::move(r);
				has_done_ = true;
				spare_ = std::move(scratch);
			}
		}
	}
	// Main thread: the finished result, if any and if `admit(result)` lets it in (its upload's bytes); the slot is then free
	// for the next job.
	template <typename Admit>
	bool take(Raster &out, Admit &&admit) {
		std::lock_guard<std::mutex> lk(mutex_);
		if (!has_done_ || !admit(static_cast<const Raster &>(done_))) return false;
		out = std::move(done_);
		done_ = Raster();
		has_done_ = false;
		busy_ = false;
		return true;
	}
	void reset() {
		std::unique_ptr<Scratch> gone;
		Raster dropped;
		{
			std::lock_guard<std::mutex> lk(mutex_);
			generation_++;
			busy_ = false;
			has_done_ = false;
			dropped = std::move(done_);
			done_ = Raster();
			gone = std::move(spare_);
		}
	}

private:
	mutable std::mutex mutex_;
	Raster done_;
	bool has_done_ = false, busy_ = false;
	uint64_t generation_ = 0;
	std::unique_ptr<Scratch> spare_;
};

} // namespace city
