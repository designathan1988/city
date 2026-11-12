// Versioned pending lane proposals: leases own only reservation metadata, never a CityState pointer or physical actor.
#pragma once
#include "core/generation.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <utility>
namespace city {
class BirthReservations {
public:
	struct Claim { float at = 0; int type = 0; };
private:
	struct LaneKey {
		int lane; uint64_t source;
		bool operator<(const LaneKey &b) const { return lane != b.lane ? lane < b.lane : source < b.source; }
	};
	struct State { std::mutex mutex; std::map<LaneKey, std::map<uint64_t, Claim>> lanes; };
public:
	class Lease {
	public:
		Lease(const Lease &) = delete; Lease &operator=(const Lease &) = delete;
		~Lease() {
			std::lock_guard<std::mutex> guard(state_->mutex);
			auto row = state_->lanes.find(lane_); if (row == state_->lanes.end()) return;
			row->second.erase(token_); if (row->second.empty()) state_->lanes.erase(row);
		}
	private:
		friend class BirthReservations;
		Lease(std::shared_ptr<State> state, LaneKey lane, uint64_t token) : state_(std::move(state)), lane_(lane), token_(token) {}
		std::shared_ptr<State> state_; LaneKey lane_; uint64_t token_;
	};
	BirthReservations() : state_(std::make_shared<State>()) {}
	BirthReservations(BirthReservations &&) = default; BirthReservations &operator=(BirthReservations &&) = default;
	BirthReservations(const BirthReservations &) = delete; BirthReservations &operator=(const BirthReservations &) = delete;
	std::shared_ptr<const Lease> reserve(int lane, uint64_t source, float at, int type) {
		const LaneKey key{lane, source}; const uint64_t token = next_generation();
		auto lease = std::shared_ptr<const Lease>(new Lease(state_, key, token));
		{ std::lock_guard<std::mutex> guard(state_->mutex); state_->lanes[key].emplace(token, Claim{at, type}); }
		return lease;
	}
	void put(int lane, uint64_t source, int actor, float at, int type) { actors_[actor] = reserve(lane, source, at, type); }
	void erase(int actor) { actors_.erase(actor); }
	template<class F> bool any(int lane, uint64_t source, F &&predicate) const {
		std::lock_guard<std::mutex> guard(state_->mutex);
		const auto row = state_->lanes.find({lane, source}); if (row == state_->lanes.end()) return false;
		for (const auto &entry : row->second) if (predicate(entry.second)) return true; // metadata-only predicate, never a write/reentrant callback
		return false;
	}
	size_t count(int lane, uint64_t source) const {
		std::lock_guard<std::mutex> guard(state_->mutex); const auto row = state_->lanes.find({lane, source});
		return row == state_->lanes.end() ? 0 : row->second.size();
	}
private:
	std::shared_ptr<State> state_; // old leases can retire their own old metadata after a world reset
	std::map<int, std::shared_ptr<const Lease>> actors_; // only actor-reserved births; no cycle back from State
};
} // namespace city
