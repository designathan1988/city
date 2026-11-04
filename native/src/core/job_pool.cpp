// Priority job pool: see core/job_pool.h.
#include "core/job_pool.h"

#include <algorithm>
#include <atomic>
#include <memory>

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

void JobPool::submit(JobKey key, float priority, std::function<void()> work) {
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

void JobPool::wait_idle() {
	std::unique_lock<std::mutex> lk(m_);
	idle_cv_.wait(lk, [&]() { return idle(); });
}

void JobPool::loop() {
	in_pool = true;
	for (;;) {
		std::function<void()> work;
		{
			std::unique_lock<std::mutex> lk(m_);
			cv_.wait(lk, [&]() { return stop_ || !q_.empty() || !helpers_.empty(); });
			if (stop_ && q_.empty() && helpers_.empty()) return;
			if (!helpers_.empty()) {
				work = std::move(helpers_.front());
				helpers_.pop_front();
			} else {
				Item item = q_.top();
				q_.pop();
				auto it = latest_.find(item.key);
				if (it == latest_.end() || it->second != item.seq) { // superseded while queued
					replaced_++;
					if (idle()) idle_cv_.notify_all();
					continue;
				}
				work = std::move(item.work);
			}
			++busy_;
		}
		work();
		{
			std::lock_guard<std::mutex> lk(m_);
			--busy_;
			if (idle()) idle_cv_.notify_all();
		}
	}
}

} // namespace city
