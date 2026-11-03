// Priority job pool: see core/job_pool.h.
#include "core/job_pool.h"

#include <algorithm>
#include <atomic>

namespace city {

namespace {
thread_local bool in_pool = false;
}

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

void JobPool::submit(int64_t key, float priority, std::function<void()> work) {
	{
		std::lock_guard<std::mutex> lk(m_);
		latest_[key] = ++seq_;
		q_.push(Item{ priority, seq_, key, std::move(work) });
	}
	cv_.notify_one();
}

void JobPool::parallel_for(size_t n, const std::function<void(size_t)> &fn) {
	if (n == 0) return;
	if (n == 1 || in_pool || threads_.empty()) {
		for (size_t i = 0; i < n; i++) fn(i);
		return;
	}
	std::atomic<size_t> next{ 0 };
	std::mutex done_m;
	std::condition_variable done_cv;
	size_t done = 0;
	const size_t workers = std::min(n, threads_.size());
	for (size_t k = 0; k < workers; k++)
		submit(-1 - int64_t(k), 0.f, [&]() {
			for (size_t i = next++; i < n; i = next++) fn(i);
			std::lock_guard<std::mutex> lk(done_m);
			if (++done == workers) done_cv.notify_one();
		});
	std::unique_lock<std::mutex> lk(done_m);
	done_cv.wait(lk, [&]() { return done == workers; });
}

void JobPool::wait_idle() {
	std::unique_lock<std::mutex> lk(m_);
	idle_cv_.wait(lk, [&]() { return q_.empty() && busy_ == 0; });
}

void JobPool::loop() {
	in_pool = true;
	for (;;) {
		Item item;
		{
			std::unique_lock<std::mutex> lk(m_);
			cv_.wait(lk, [&]() { return stop_ || !q_.empty(); });
			if (stop_ && q_.empty()) return;
			item = q_.top();
			q_.pop();
			auto it = latest_.find(item.key);
			if (it == latest_.end() || it->second != item.seq) continue; // superseded while queued
			++busy_;
		}
		item.work();
		{
			std::lock_guard<std::mutex> lk(m_);
			--busy_;
			if (q_.empty() && busy_ == 0) idle_cv_.notify_all();
		}
	}
}

} // namespace city
