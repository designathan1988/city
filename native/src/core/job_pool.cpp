// Priority job pool: see core/job_pool.h.
#include "core/job_pool.h"

#include "core/memory.h"

#include <algorithm>
#include <atomic>
#include <memory>

namespace city {

namespace {
thread_local bool in_pool = false;
// The keyed job running on this thread (running_current).
thread_local JobPool *running_pool = nullptr;
thread_local JobKey running_key;
thread_local uint64_t running_seq = 0;
} // namespace

JobPool::JobPool(unsigned threads, std::chrono::milliseconds linger) : linger_(linger) {
	const unsigned hw = std::thread::hardware_concurrency();
	const unsigned n = threads ? threads : std::max(2u, hw > 1 ? hw - 1 : 2u);
	workers_.reserve(n);
	idle_.reserve(n);
	for (unsigned i = 0; i < n; i++) workers_.push_back(std::make_unique<Worker>()); // places: a thread starts when work comes
}

JobPool::~JobPool() {
	{
		std::lock_guard<std::mutex> lk(m_);
		stop_ = true;
		for (Worker *w : idle_) w->cv.notify_one();
	}
	for (const std::unique_ptr<Worker> &w : workers_)
		if (w->thread.joinable()) w->thread.join();
}

std::vector<JobPool::Worker *> JobPool::call(size_t count) {
	std::vector<Worker *> places;
	while (count > 0 && !idle_.empty()) {
		Worker *w = idle_.back();
		idle_.pop_back();
		w->cv.notify_one();
		count--;
	}
	for (const std::unique_ptr<Worker> &w : workers_) {
		if (count == 0) break;
		if (w->live) continue;
		w->live = true; // the place is taken: nobody else starts it
		places.push_back(w.get());
		count--;
	}
	return places;
}

void JobPool::start(const std::vector<Worker *> &places) {
	for (Worker *w : places) {
		w->thread = std::thread([this, w]() { loop(*w); });
	}
}

uint64_t JobPool::submit(JobKey key, float priority, std::function<void()> work) {
	std::function<void()> superseded; // the older queued closure of the key: destroyed after the lock is let go
	std::vector<Worker *> places;
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(m_);
		seq = ++seq_;
		Slot &slot = slots_[key];
		if (slot.work) {
			superseded = std::move(slot.work);
			replaced_++;
		}
		slot.seq = seq;
		slot.work = std::move(work);
		newest_[key] = seq;
		q_.push(Item{ priority, seq, key });
		places = call(1);
	}
	start(places);
	return seq;
}

bool JobPool::current(JobKey key, uint64_t seq) {
	std::lock_guard<std::mutex> lk(m_);
	auto it = newest_.find(key);
	return it != newest_.end() && it->second == seq;
}

bool JobPool::running_current() {
	if (running_pool != this) return true;
	return current(running_key, running_seq);
}

bool JobPool::running(JobKey &key) {
	if (!running_pool) return false;
	key = running_key;
	return true;
}

const char *job_lane_name(JobLane lane) {
	switch (lane) {
		case JobLane::ROADS: return "roads";
		case JobLane::ROADS_FAR: return "roads_far";
		case JobLane::VERGES: return "verges";
		case JobLane::STRUCTURES: return "structures";
		case JobLane::PROPS: return "props";
		case JobLane::BUILDING: return "building";
		case JobLane::BUILDING_SUPER: return "building_super";
		case JobLane::TREES_NEAR: return "trees_near";
		case JobLane::TREES_FAR: return "trees_far";
		case JobLane::COVER: return "cover";
		case JobLane::ZONES: return "zones";
		case JobLane::FOG: return "fog";
		case JobLane::CANYON: return "canyon";
		case JobLane::SHORE: return "shore";
		case JobLane::RUINS: return "ruins";
		case JobLane::WATER: return "water";
		case JobLane::SIGNATURES: return "signatures";
	}
	return "";
}

void JobPool::parallel_for(size_t n, const std::function<void(size_t)> &fn) {
	if (n == 0) return;
	if (n == 1 || in_pool || workers_.empty()) {
		for (size_t i = 0; i < n; i++) fn(i);
		return;
	}
	// A helper that starts after the caller closed the batch returns without touching `fn` (the caller's frame).
	struct Batch {
		std::atomic<size_t> next{ 0 };
		std::mutex m;
		std::condition_variable cv;
		size_t running = 0;
		bool closed = false;
		const std::function<void(size_t)> *fn = nullptr;
		size_t n = 0;
	};
	auto batch = std::make_shared<Batch>();
	batch->fn = &fn;
	batch->n = n;
	const size_t helpers = std::min(n, workers_.size() + 1) - 1;
	std::vector<Worker *> places;
	{
		std::lock_guard<std::mutex> lk(m_);
		for (size_t k = 0; k < helpers; k++)
			helpers_.push_back([batch]() {
				{
					std::lock_guard<std::mutex> lk(batch->m);
					if (batch->closed) return;
					batch->running++;
				}
				for (size_t i = batch->next++; i < batch->n; i = batch->next++) (*batch->fn)(i);
				std::lock_guard<std::mutex> lk(batch->m);
				if (--batch->running == 0) batch->cv.notify_all();
			});
		places = call(helpers); // a worker a helper, never the whole pool
	}
	start(places);
	for (size_t i = batch->next++; i < n; i = batch->next++) fn(i);
	std::unique_lock<std::mutex> lk(batch->m);
	batch->closed = true;
	batch->cv.wait(lk, [&]() { return batch->running == 0; });
}

size_t JobPool::tracked() {
	std::lock_guard<std::mutex> lk(m_);
	return slots_.size();
}

void JobPool::freed() {
	std::lock_guard<std::mutex> lk(m_);
	freed_++;
	for (Worker *w : idle_)
		if (w->holds) w->cv.notify_one(); // only those with pages have anything to look at
}

void JobPool::wait_idle() {
	std::unique_lock<std::mutex> lk(m_);
	idle_cv_.wait(lk, [&]() { return idle(); });
}

void JobPool::loop(Worker &me) {
	in_pool = true;
	// A look at its heap is due when it holds pages and something may have been freed there since the last look: its own work
	// ended (`fresh`), or the pool was told of a free (freed_ moved past `looked`). Never sooner than a linger after the last.
	bool fresh = false;
	uint64_t looked = freed_;
	auto next_look = std::chrono::steady_clock::now();
	// held but while a job runs or the heap is looked at: a worker that finishes stands on the idle stack before anybody can submit again
	std::unique_lock<std::mutex> lk(m_);
	for (;;) {
		while (!ready()) {
			idle_.push_back(&me);
			bool timed_out = false;
			if (me.holds && (fresh || freed_ != looked)) timed_out = me.cv.wait_until(lk, next_look) == std::cv_status::timeout;
			else me.cv.wait(lk);
			const auto at = std::find(idle_.begin(), idle_.end(), &me); // whoever called it took it off; a timeout or a free did not
			const bool uncalled = at != idle_.end();
			if (uncalled) idle_.erase(at);
			if (timed_out && uncalled && !ready()) { // nothing came: what was freed in its heap goes back to the system
				fresh = false;
				looked = freed_;
				lk.unlock();
				release_freed_memory();
				const bool still = heap_held() > 0;
				lk.lock();
				me.holds = still;
				next_look = std::chrono::steady_clock::now() + linger_;
			}
		}
		if (stop_ && q_.empty() && helpers_.empty()) return;
		std::function<void()> work;
		bool keyed = false;
		JobKey key;
		uint64_t seq = 0;
		if (!helpers_.empty()) {
			work = std::move(helpers_.front());
			helpers_.pop_front();
		} else {
			const Item item = q_.top();
			q_.pop();
			auto it = slots_.find(item.key);
			if (it == slots_.end() || it->second.seq != item.seq) { // superseded while queued: its work went with the newer submit
				if (idle()) idle_cv_.notify_all();
				continue;
			}
			work = std::move(it->second.work);
			slots_.erase(it); // its newest job runs: an older heap entry finds no slot of its seq and is dropped
			keyed = true;
			key = item.key;
			seq = item.seq;
		}
		++busy_;
		lk.unlock();
		if (keyed) {
			running_pool = this;
			running_key = key;
			running_seq = seq;
		}
		work();
		running_pool = nullptr;
		work = nullptr; // the closure's captures go before the pool's lock is taken
		lk.lock();
		if (keyed) {
			auto it = newest_.find(key);
			if (it != newest_.end() && it->second == seq) newest_.erase(it);
		}
		--busy_;
		me.holds = true;
		fresh = true;
		next_look = std::chrono::steady_clock::now() + linger_;
		if (idle()) idle_cv_.notify_all();
	}
}

} // namespace city
