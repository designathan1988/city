// Priority job pool: see core/job_pool.h.
#include "core/job_pool.h"

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

JobPool::JobPool(unsigned threads) {
	const unsigned hw = std::thread::hardware_concurrency();
	const unsigned n = threads ? threads : std::max(2u, hw > 1 ? hw - 1 : 2u);
	threads_.reserve(n);
	for (unsigned i = 0; i < n; i++) threads_.emplace_back([this]() { loop(); });
}

JobPool::~JobPool() {
	{
		std::lock_guard<std::mutex> lk(m_);
		stop_ = true;
	}
	cv_.notify_all();
	for (std::thread &t : threads_)
		if (t.joinable()) t.join();
}

uint64_t JobPool::submit(JobKey key, float priority, std::function<void()> work) {
	std::function<void()> superseded; // the older queued closure of the key: destroyed after the lock is let go
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
	}
	cv_.notify_one();
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

void JobPool::parallel_for(size_t n, const std::function<void(size_t)> &fn) {
	if (n == 0) return;
	if (n == 1 || in_pool || threads_.empty()) {
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
	const size_t helpers = std::min(n, threads_.size() + 1) - 1;
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
	}
	cv_.notify_all();
	for (size_t i = batch->next++; i < n; i = batch->next++) fn(i);
	std::unique_lock<std::mutex> lk(batch->m);
	batch->closed = true;
	batch->cv.wait(lk, [&]() { return batch->running == 0; });
}

size_t JobPool::tracked() {
	std::lock_guard<std::mutex> lk(m_);
	return slots_.size();
}

void JobPool::pause() {
	std::unique_lock<std::mutex> lk(m_);
	paused_ = true;
	idle_cv_.wait(lk, [&]() { return busy_ == 0; });
}

void JobPool::resume() {
	{
		std::lock_guard<std::mutex> lk(m_);
		paused_ = false;
	}
	cv_.notify_all();
}

void JobPool::wait_idle() {
	std::unique_lock<std::mutex> lk(m_);
	idle_cv_.wait(lk, [&]() { return idle(); });
}

void JobPool::loop() {
	in_pool = true;
	for (;;) {
		std::function<void()> work;
		bool keyed = false;
		JobKey key;
		uint64_t seq = 0;
		{
			std::unique_lock<std::mutex> lk(m_);
			cv_.wait(lk, [&]() { return stop_ || (!paused_ && (!q_.empty() || !helpers_.empty())); });
			if (stop_ && q_.empty() && helpers_.empty()) return;
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
		}
		if (keyed) {
			running_pool = this;
			running_key = key;
			running_seq = seq;
		}
		work();
		running_pool = nullptr;
		work = nullptr; // the closure's captures go before the pool's lock is taken
		{
			std::lock_guard<std::mutex> lk(m_);
			if (keyed) {
				auto it = newest_.find(key);
				if (it != newest_.end() && it->second == seq) newest_.erase(it);
			}
			--busy_;
			if (busy_ == 0) idle_cv_.notify_all(); // wait_idle and pause wake on it
		}
	}
}

} // namespace city
