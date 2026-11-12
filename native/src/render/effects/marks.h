// What a blast leaves on the ground, flat quads just over it in one MultiMesh (render/shaders/marks.gdshader draws each
// from its kind): the craters of the damage state (drawn again whenever the state changes: undo, redo, open), soot and
// blood kept (the oldest go first), and the shock rings while they race out.
#pragma once

#include "render/effects/mm_layer.h"
#include "sim/demolition/damage_state.h"

#include <algorithm>
#include <deque>
#include <cmath>
#include <map>
#include <tuple>
#include <vector>

namespace city {

// Mark kinds, the shader's.
enum MarkKind : int { MARK_CRATER_HARD = 0, MARK_CRATER_EARTH, MARK_SOOT, MARK_POOL, MARK_SPLAT, MARK_DROPS, MARK_RING };

struct Mark {
	float x = 0, y = 0, z = 0, size = 0, seed = 0;
	int kind = 0;
	double born = 0, life = 0;
};

class Marks {
public:
	void init(const godot::RID &scenario, const godot::RID &material, int capacity, int soot_kept, int blood_kept, float sector_m = 0);
	void reset();
	void release();
	int blood() const { return int(blood_.size()); }
	// The craters as the state has them (`ground_of`: the terrain's height at a point, before any digging for earth).
	template <typename F, typename G>
	void set_craters(const std::vector<Crater> &list, F &&ground_of, G &&ground_stamp) {
		auto previous = std::move(crater_cache_);
		crater_cache_.clear();
		craters_.clear();
		total_craters_ = list.size();
		const size_t count = std::min(list.size(), size_t(std::max(0, capacity_)));
		craters_.reserve(count);
		// Craters already precede soot/blood/rings in the capacity policy. Invisible tail records need no render preparation.
		for (size_t i = 0; i < count; i++) {
			const Crater &c = list[i];
			Mark m;
			m.kind = c.hard ? MARK_CRATER_HARD : MARK_CRATER_EARTH;
			m.x = c.x; m.z = c.z; m.size = c.radius * 2.f * CRATER_SCALE; m.seed = float(c.seed & SEED_MASK) / float(SEED_MASK);
			const uint64_t stamp = ground_stamp(c.x, c.z, m.size * .5f);
			const CraterKey key(c.x, c.z, c.radius, c.hard, c.seed);
			const bool cacheable = std::isfinite(c.x) && std::isfinite(c.z) && std::isfinite(c.radius);
			const auto found = cacheable ? previous.find(key) : previous.end();
			if (found != previous.end() && found->second.stamp == stamp) m = found->second.mark;
			else m.y = top_ground(c.x, c.z, m.size * .5f, ground_of) + SURFACE_LIFT_M;
			if (cacheable) crater_cache_[key] = { m, stamp };
			craters_.push_back(m);
		}
		dirty_ = true;
	}
	// One flat quad over rolling ground is laid at the highest ground under it (`half` metres out each way), so no part of it is buried.
	template <typename F>
	static float top_ground(float x, float z, float half, F &&ground_of) {
		float top = ground_of(x, z);
		for (int j = -RELIEF_GRID; j <= RELIEF_GRID; j++)
			for (int i = -RELIEF_GRID; i <= RELIEF_GRID; i++)
				top = std::max(top, ground_of(x + half * float(i) / float(RELIEF_GRID), z + half * float(j) / float(RELIEF_GRID)));
		return top;
	}
	void add_soot(const Mark &m);
	void add_blood(const Mark &m);
	void ring(const Mark &m);
	size_t craters() const { return total_craters_; }
	// Writes what changed (and the rings' ends) at the effects clock `now`.
	void update(double now);
	static constexpr float SURFACE_LIFT_M = 0.06f;
	static constexpr int RELIEF_GRID = 4; // the ground is sampled on a grid of (2 * this + 1)^2 points across a decal

private:
	static constexpr float CRATER_SCALE = 2.4f; // the quad is wider than the hole: the scorch and the broken lip round it
	static constexpr uint32_t SEED_MASK = 0xffffu;
	MmLayer layer_;
	godot::RID material_;
	using CraterKey = std::tuple<float, float, float, uint8_t, uint32_t>;
	struct PreparedCrater { Mark mark; uint64_t stamp = 0; };
	std::map<CraterKey, PreparedCrater> crater_cache_; // only the admitted prefix; no hash collisions in identity
	size_t total_craters_ = 0;
	std::vector<Mark> craters_;
	std::deque<Mark> soot_, blood_;
	std::vector<Mark> rings_;
	size_t soot_kept_ = 0, blood_kept_ = 0;
	int capacity_ = 0, shown_ = 0;
	bool dirty_ = false;
	void write(const Mark &m, int slot);
};

} // namespace city
