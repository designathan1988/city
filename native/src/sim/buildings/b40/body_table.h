// Immutable authored versions, addressed by generation-local ids. Live records, history, damage and jobs pin versions;
// the table is a weak lookup. New versions stay pending only until their command/load has installed its owning roots.
#pragma once

#include "sim/buildings/b40/types.h"
#include "sim/buildings/b40/body_memory.h"
#include "sim/buildings/b40/blocks.h"
#include "core/generation.h"

#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace city::b40 {

using BodyRef = std::shared_ptr<const Body>;

// Version-owned weak observer: derived values never keep the version itself alive.
struct BodyDerivedOwner {
	virtual ~BodyDerivedOwner() = default;
	virtual void source_retired(const Body *source) = 0;
};

struct BodyInput {
	uint64_t generation = 0;
	BodyRef edited;
	BodyRef mass; // alias of the same immutable version root, already resolved once before publication
};

class BodyTable {
	struct Version {
		Body authored, mass;
		mutable std::mutex derived_mutex;
		mutable std::weak_ptr<BodyDerivedOwner> derived;
	};
	struct State {
		mutable std::shared_mutex mutex;
		const uint64_t generation = next_generation();
		uint32_t last_id = 0;
		std::unordered_map<uint32_t, std::weak_ptr<const Body>> items;
		std::unordered_map<uint32_t, BodyRef> pending;
	};
	struct VersionRelease {
		std::weak_ptr<State> owner;
		uint32_t id;
		size_t bytes;
		const Version *version;
		void operator()(const Version *retired) const {
			// No other strong root remains: no reader can now register or prepare a value for this source.
			if (auto derived = retired->derived.lock()) derived->source_retired(&retired->mass);
			delete retired;
			if (const auto retained = owner.lock()) {
				std::unique_lock<std::shared_mutex> lk(retained->mutex);
				retained->items.erase(id);
			}
		}
	};
public:
	// The version's number, from 1: 0 in a record is a body made from its seed.
	uint32_t add(Body body) {
		const auto state = std::atomic_load(&state_);
		uint32_t id;
		{
			std::unique_lock<std::shared_mutex> lk(state->mutex);
			if (state->last_id == std::numeric_limits<uint32_t>::max()) std::abort(); // ids never wrap into another live version
			id = ++state->last_id;
		}
		auto value = std::make_unique<Version>();
		value->authored = std::move(body); value->mass = value->authored;
		resolve_blocks(value->mass); // source invariant, prepared once; readers never resolve the same authored version again
		const size_t bytes = body_retained_bytes(value->authored) + body_retained_bytes(value->mass)
			+ sizeof(Version) - 2 * sizeof(Body);
		const Version *raw = value.release();
		const std::shared_ptr<const Version> root(raw, VersionRelease{ state, id, bytes, raw });
		BodyRef version(root, &root->authored);
		{
			std::unique_lock<std::shared_mutex> lk(state->mutex);
			state->items.emplace(id, version);
			state->pending.emplace(id, version); // the local root also protects allocation-failure unwinding under this lock
		}
		return id;
	}
	BodyRef get(uint32_t id) const {
		return input(id).edited;
	}
	static size_t retained_bytes(const BodyRef &body) {
		if (!body) return 0;
		const auto *release = std::get_deleter<VersionRelease>(body);
		return release ? release->bytes + sizeof(VersionRelease) : body_retained_bytes(*body);
	}
	static std::shared_ptr<BodyDerivedOwner> derived_owner(const BodyRef &body, const std::shared_ptr<BodyDerivedOwner> &candidate) {
		const auto *release = body ? std::get_deleter<VersionRelease>(body) : nullptr;
		if (!release) return {};
		std::lock_guard<std::mutex> lk(release->version->derived_mutex);
		auto owner = release->version->derived.lock();
		if (!owner) { owner = candidate; release->version->derived = owner; }
		return owner;
	}
	static BodyRef mass_of(const BodyRef &body) {
		const auto *release = body ? std::get_deleter<VersionRelease>(body) : nullptr;
		return release ? BodyRef(body, &release->version->mass) : BodyRef{};
	}
	static BodyRef authored_of(const BodyRef &body) {
		const auto *release = body ? std::get_deleter<VersionRelease>(body) : nullptr;
		return release ? BodyRef(body, &release->version->authored) : body;
	}
	BodyInput input(uint32_t id) const {
		const auto state = std::atomic_load(&state_);
		std::shared_lock<std::shared_mutex> lk(state->mutex);
		const auto it = state->items.find(id);
		BodyRef authored = it == state->items.end() ? nullptr : it->second.lock();
		return { state->generation, authored, mass_of(authored) };
	}
	uint64_t generation() const { return std::atomic_load(&state_)->generation; }
	void release_pending() {
		const auto state = std::atomic_load(&state_);
		std::unordered_map<uint32_t, BodyRef> retired;
		{
			std::unique_lock<std::shared_mutex> lk(state->mutex);
			retired.swap(state->pending);
		}
		// A final root's deleter removes the weak lookup, so retire pending roots outside the table mutex.
	}
	void clear() {
		std::atomic_exchange(&state_, std::make_shared<State>());
	}

private:
	std::shared_ptr<State> state_{ std::make_shared<State>() };
};

} // namespace city::b40
