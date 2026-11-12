// A main-thread request's replaceable inbox. Workers publish only to their ticket; resetting retires the entire old inbox.
#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <utility>

namespace city {
template <typename T>
class AsyncResult {
public:
	struct Inbox {
		std::mutex mutex;
		std::optional<T> value;
		std::atomic<bool> retired{ false };
		bool finished = false; // value publication or terminal cancellation, protected by mutex
	};
	using Ticket = std::shared_ptr<Inbox>;
	// Captured by the queued closure: its final release acknowledges even a job the pool never invokes.
	// A source that removed desired keys supplies an incomplete fallback to restore those keys on cancellation.
	struct Completion {
		Ticket ticket;
		std::shared_ptr<T> fallback;
		Completion(Ticket request, std::shared_ptr<T> cancelled) : ticket(std::move(request)), fallback(std::move(cancelled)) {}
		~Completion() {
			std::lock_guard<std::mutex> lock(ticket->mutex);
			if (ticket->retired.load(std::memory_order_acquire) || ticket->finished) return;
			if (fallback) ticket->value.emplace(std::move(*fallback));
			ticket->finished = true;
		}
	};
	static std::shared_ptr<Completion> completion(const Ticket &ticket, std::shared_ptr<T> fallback = {}) {
		return std::make_shared<Completion>(ticket, std::move(fallback));
	}
	AsyncResult() = default;
	AsyncResult(const AsyncResult &) = delete;
	AsyncResult &operator=(const AsyncResult &) = delete;
	~AsyncResult() { inbox_->retired.store(true, std::memory_order_release); }
	bool pending() const { return pending_; } // owner thread only
	Ticket begin() {
		if (pending_) return {};
		// Every request gets a distinct inbox. A previous closure's late finalizer cannot finish the next request.
		inbox_->retired.store(true, std::memory_order_release);
		inbox_ = std::make_shared<Inbox>();
		pending_ = true;
		return inbox_;
	}
	void reset() {
		inbox_->retired.store(true, std::memory_order_release);
		inbox_ = std::make_shared<Inbox>();
		pending_ = false;
	}
	bool take(T &out) {
		std::lock_guard<std::mutex> lock(inbox_->mutex);
		if (!inbox_->value) {
			if (inbox_->finished) pending_ = false;
			return false;
		}
		out = std::move(*inbox_->value);
		inbox_->value.reset();
		pending_ = false;
		return true;
	}
	static bool publish(const Ticket &ticket, T value) {
		std::lock_guard<std::mutex> lock(ticket->mutex);
		if (ticket->retired.load(std::memory_order_acquire) || ticket->finished) return false;
		ticket->value.emplace(std::move(value));
		ticket->finished = true;
		return true;
	}
	static bool retired(const Ticket &ticket) { return ticket->retired.load(std::memory_order_acquire); }

private:
	Ticket inbox_ = std::make_shared<Inbox>();
	bool pending_ = false;
};
} // namespace city
