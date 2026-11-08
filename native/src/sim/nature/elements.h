// Elements: the small things the player lays with a brush over the ground (stones, grass, flowers...) and the effects
// that give off smoke, fire or spray. Kept per 256 m chunk, the only way they are read; which kinds exist and which of
// them are effects comes from data/catalog/nature.json "elements".
#pragma once

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace city {

struct ElementItem {
	float x = 0, z = 0;
	float size = 0; // metres across (a prop) or the spread (an effect)
	float yaw = 0;
	float seed = 0; // 0..1: which variant
	float intensity = 0; // effects: 0..1, how much it gives off
	int32_t kind = 0;
	bool operator==(const ElementItem &o) const {
		return x == o.x && z == o.z && size == o.size && yaw == o.yaw && seed == o.seed && intensity == o.intensity && kind == o.kind;
	}
	// A total order by value: the save writes the items in it (ByteWriter::put_set), so the same items give the same bytes
	// whichever chunk order and removals they went through.
	static bool before(const ElementItem &a, const ElementItem &b) {
		return std::tie(a.x, a.z, a.kind, a.size, a.yaw, a.seed, a.intensity) < std::tie(b.x, b.z, b.kind, b.size, b.yaw, b.seed, b.intensity);
	}
};

class Elements {
public:
	void clear();
	size_t count() const { return count_; }
	void add(const ElementItem &e);
	bool remove(const ElementItem &e); // that very item
	const std::vector<ElementItem> *in_chunk(int64_t key) const;
	// Calls fn for the items within `r` of (x, z).
	template <typename F>
	void for_near(float x, float z, float r, F &&fn) const {
		for (int64_t key : chunks_near(x, z, r)) {
			const std::vector<ElementItem> *c = in_chunk(key);
			if (!c) continue;
			for (const ElementItem &e : *c)
				if ((e.x - x) * (e.x - x) + (e.z - z) * (e.z - z) <= r * r) fn(e);
		}
	}
	std::vector<ElementItem> all() const;

private:
	std::vector<int64_t> chunks_near(float x, float z, float r) const;
	std::unordered_map<int64_t, std::vector<ElementItem>> cells_;
	size_t count_ = 0;
};

} // namespace city
