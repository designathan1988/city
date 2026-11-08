// The bodies the player edited, each version kept whole and never changed, and held by whoever names it: a record
// (`Building::body`), a step of the undo history, a job's copy of the records. The last holder to let go frees it, so a
// version the history retired and no building stands on costs nothing. A body made from a seed is not a version, so a
// building nobody edited costs nothing either. Each version carries a stamp that is never given twice, in this city or the
// next: what a cache of derived bodies names it by.
#pragma once

#include "sim/buildings/b40/types.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

namespace city::b40 {

struct BodyVersion {
	Body body;
	uint64_t stamp = 0;
};

// A held version, or none (a body made from the record's seed). Compares with 0 and tests as a bool, as the number it replaced.
class BodyRef {
public:
	BodyRef() = default;
	BodyRef(std::nullptr_t) {} // 0 / nullptr: made from the seed
	explicit operator bool() const { return v_ != nullptr; }
	bool operator==(std::nullptr_t) const { return v_ == nullptr; }
	bool operator!=(std::nullptr_t) const { return v_ != nullptr; }
	const Body &operator*() const { return v_->body; }
	const Body *operator->() const { return &v_->body; }
	std::shared_ptr<const Body> shared() const { return std::shared_ptr<const Body>(v_, &v_->body); } // held as long as the version
	uint64_t stamp() const { return v_ ? v_->stamp : 0; } // 0: none

private:
	friend class BodyTable;
	explicit BodyRef(std::shared_ptr<const BodyVersion> v) : v_(std::move(v)) {}
	std::shared_ptr<const BodyVersion> v_;
};

// The maker of versions: the one place a stamp is given.
class BodyTable {
public:
	BodyRef add(Body body) {
		return BodyRef(std::make_shared<const BodyVersion>(BodyVersion{ std::move(body), next_stamp() }));
	}

private:
	static uint64_t next_stamp() {
		static std::atomic<uint64_t> next{ 1 };
		return next.fetch_add(1, std::memory_order_relaxed);
	}
};

// What names a record's body exactly (record.h body_key): an edited one by its version's stamp, one made from the seed by all it
// is made from (seed, zone, floors, the lot's site, frontage and depth, the last two to the bit, as the generator reads them). The function and
// the seed are in both: what is derived from a body with them (its look, its rooms, its purpose) differs by them. Two records
// with the same key have the same body and the same derived things, wherever they stand.
struct BodyKey {
	uint64_t stamp = 0; // the edited version (BodyRef::stamp), 0 for a body made from the seed
	uint32_t seed = 0;
	uint32_t w = 0, d = 0; // frontage and depth: the floats' bits
	uint16_t floors = 0;
	uint8_t zone = 0, fn = 0;
	uint8_t site = 0; // the lot's LotSite bits: a corner or an uphill lot lays out another body
	bool operator==(const BodyKey &o) const {
		return stamp == o.stamp && seed == o.seed && w == o.w && d == o.d && floors == o.floors && zone == o.zone && fn == o.fn && site == o.site;
	}
	bool operator!=(const BodyKey &o) const { return !(*this == o); }
};
struct BodyKeyHash {
	static constexpr uint64_t MIX = 0x9e3779b97f4a7c15ull; // 2^64 over the golden ratio
	static constexpr int SHIFT = 31, HALF_BITS = 32, BYTE_BITS = 8; // the product's high half folded down; a word's halves; a byte
	static uint64_t mix(uint64_t h, uint64_t v) {
		h = (h ^ v) * MIX;
		return h ^ (h >> SHIFT);
	}
	size_t operator()(const BodyKey &k) const {
		uint64_t h = mix(k.stamp, uint64_t(k.seed) | (uint64_t(uint32_t(k.w)) << HALF_BITS));
		h = mix(h, uint64_t(uint32_t(k.d)) | (uint64_t(k.floors) << HALF_BITS));
		return size_t(mix(h, uint64_t(k.zone) | (uint64_t(k.fn) << BYTE_BITS) | (uint64_t(k.site) << (2 * BYTE_BITS))));
	}
};

// The memory a body holds besides itself: its lists, and its maps' entries with their nodes.
inline size_t heap_bytes(const Body &b) {
	constexpr size_t MAP_NODE_BYTES = 4 * sizeof(void *); // a hash or tree node's links and hash, past its pair
	size_t n = b.paints.capacity() * sizeof(Paint) + b.volumes.capacity() * sizeof(Volume) + b.elements.capacity() * sizeof(LotElement) +
			b.cores.capacity() * sizeof(Core);
	for (const Volume &v : b.volumes) {
		n += v.outline.capacity() * sizeof(V2) + v.fill.capacity() + v.reliefs.capacity() * sizeof(Relief) + v.gear.capacity() * sizeof(RoofPart);
		n += (v.bays.size() + v.side_fill.size()) * (sizeof(std::pair<uint32_t, uint8_t>) + MAP_NODE_BYTES);
		n += v.storey_paint.size() * (sizeof(std::pair<uint32_t, int>) + MAP_NODE_BYTES);
	}
	for (const auto &floor : b.furnishing) n += sizeof(floor) + MAP_NODE_BYTES + floor.second.capacity() * sizeof(PlacedPiece);
	return n;
}

} // namespace city::b40
