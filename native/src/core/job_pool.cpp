// Priority job pool: see core/job_pool.h.
#include "core/job_pool.h"

#include <algorithm>
#include <atomic>
#include <memory>

namespace city {

namespace {
thread_local bool in_pool = false;
thread_local std::shared_ptr<std::atomic<bool>> current_cancellation;
}

JobPool::JobPool(unsigned threads) {
	const unsigned hw = std::thread::hardware_concurrency();
	const unsigned n = threads ? threads : std::max(2u, hw > 1 ? hw - 1 : 2u);
	threads_.reserve(n);
	for (unsigned i = 0; i < n; i++) threads_.emplace_back([this]() { loop(); });
}

JobPool::~JobPool() {
	cancel_all();
	{
		std::lock_guard<std::mutex> lk(m_);
		stop_ = true;
	}
	cv_.notify_all();
	for (std::thread &t : threads_)
		if (t.joinable()) t.join();
}

void JobPool::submit(JobKey key, float priority, std::function<void()> work) {
	Item retired;
	{
		std::lock_guard<std::mutex> lk(m_);
		auto previous = current_.find(key);
		if (previous != current_.end()) previous->second->store(true, std::memory_order_relaxed);
		auto queued = queued_.find(key);
		if (queued != queued_.end()) {
			retired = remove_at(queued->second);
			replaced_++;
		}
		auto cancellation = std::make_shared<std::atomic<bool>>(false);
		current_[key] = cancellation;
		queued_[key] = q_.size();
		q_.push_back(Item{ priority, ++seq_, key, std::move(work), std::move(cancellation) });
		sift_up(q_.size() - 1);
	}
	cv_.notify_one();
}

bool JobPool::cancelled() {
	return current_cancellation && current_cancellation->load(std::memory_order_relaxed);
}

void JobPool::swap_items(size_t a, size_t b) {
	std::swap(q_[a], q_[b]);
	queued_[q_[a].key] = a;
	queued_[q_[b].key] = b;
}

void JobPool::sift_up(size_t at) {
	while (at != 0) {
		const size_t parent = (at - 1) / 2;
		if (!Later{}(q_[parent], q_[at])) break;
		swap_items(parent, at);
		at = parent;
	}
}

void JobPool::sift_down(size_t at) {
	for (;;) {
		size_t first = at * 2 + 1;
		if (first >= q_.size()) return;
		const size_t second = first + 1;
		if (second < q_.size() && Later{}(q_[first], q_[second])) first = second;
		if (!Later{}(q_[at], q_[first])) return;
		swap_items(at, first);
		at = first;
	}
}

JobPool::Item JobPool::remove_at(size_t at) {
	Item removed = std::move(q_[at]);
	queued_.erase(removed.key);
	if (at == q_.size() - 1) {
		q_.pop_back();
		return removed;
	}
	q_[at] = std::move(q_.back());
	q_.pop_back();
	queued_[q_[at].key] = at;
	if (at != 0 && Later{}(q_[(at - 1) / 2], q_[at])) sift_up(at);
	else sift_down(at);
	return removed;
}

void JobPool::cancel(JobKey key) {
	Item retired;
	{
		std::lock_guard<std::mutex> lk(m_);
		auto current = current_.find(key);
		if (current != current_.end()) {
			current->second->store(true, std::memory_order_relaxed);
			current_.erase(current);
		}
		auto queued = queued_.find(key);
		if (queued != queued_.end()) retired = remove_at(queued->second);
		if (idle()) idle_cv_.notify_all();
	}
}

void JobPool::cancel_all() {
	std::vector<Item> retired;
	{
		std::lock_guard<std::mutex> lk(m_);
		for (const auto &entry : current_) entry.second->store(true, std::memory_order_relaxed);
		current_.clear();
		queued_.clear();
		retired.swap(q_);
		if (idle()) idle_cv_.notify_all();
	}
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
	return queued_.size();
}

bool JobPool::drained() {
	std::lock_guard<std::mutex> lk(m_);
	return idle();
}

void JobPool::wait_idle() {
	std::unique_lock<std::mutex> lk(m_);
	idle_cv_.wait(lk, [&]() { return idle(); });
}

void JobPool::loop() {
	in_pool = true;
	for (;;) {
		std::function<void()> work;
		JobKey key;
		std::shared_ptr<std::atomic<bool>> cancellation;
		{
			std::unique_lock<std::mutex> lk(m_);
			cv_.wait(lk, [&]() { return stop_ || !q_.empty() || !helpers_.empty(); });
			if (stop_ && q_.empty() && helpers_.empty()) return;
			if (!helpers_.empty()) {
				work = std::move(helpers_.front());
				helpers_.pop_front();
			} else {
				Item item = remove_at(0);
				key = item.key;
				cancellation = std::move(item.cancelled);
				work = std::move(item.work);
			}
			++busy_;
		}
		current_cancellation = cancellation;
		if (!cancelled()) work();
		current_cancellation.reset();
		work = {}; // release captures outside m_ and before notifying wait_idle
		{
			std::lock_guard<std::mutex> lk(m_);
			if (cancellation) {
				auto current = current_.find(key);
				if (current != current_.end() && current->second == cancellation) current_.erase(current);
			}
			--busy_;
			if (idle()) idle_cv_.notify_all();
		}
	}
}

} // namespace city
