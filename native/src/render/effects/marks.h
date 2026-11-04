// What a blast leaves on the ground, flat quads just over it in one MultiMesh (render/shaders/marks.gdshader draws each
// from its kind): the craters of the damage state (drawn again whenever the state changes: undo, redo, open), soot and
// blood kept (the oldest go first), and the shock rings while they race out.
#pragma once

#include "render/effects/mm_layer.h"
#include "sim/demolition/damage_state.h"

#include <algorithm>
#include <deque>
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
	void init(const godot::RID &scenario, const godot::RID &material, int capacity, int soot_kept, int blood_kept);
	void reset();
	void release();
	// The craters as the state has them (`ground_of`: the terrain's height at a point, before any digging for earth).
	template <typename F>
	void set_craters(const std::vector<Crater> &list, F &&ground_of) {
		craters_.clear();
		for (const Crater &c : list) {
			Mark m;
			m.kind = c.hard ? MARK_CRATER_HARD : MARK_CRATER_EARTH;
			m.x = c.x; m.z = c.z; m.size = c.radius * 2.f * CRATER_SCALE; m.seed = float(c.seed & SEED_MASK) / float(SEED_MASK);
			m.y = top_ground(c.x, c.z, m.size * 0.5f, ground_of) + SURFACE_LIFT_M; // the pit the dig leaves lies below it
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
	size_t craters() const { return craters_.size(); }
	// Writes what changed (and the rings' ends) at the effects clock `now`.
	void update(double now);
	static constexpr float SURFACE_LIFT_M = 0.06f;
	static constexpr int RELIEF_GRID = 4; // the ground is sampled on a grid of (2 * this + 1)^2 points across a decal

private:
	static constexpr float CRATER_SCALE = 2.4f; // the quad is wider than the hole: the scorch and the broken lip round it
	static constexpr uint32_t SEED_MASK = 0xffffu;
	MmLayer layer_;
	godot::RID material_;
	std::vector<Mark> craters_;
	std::deque<Mark> soot_, blood_;
	std::vector<Mark> rings_;
	size_t soot_kept_ = 0, blood_kept_ = 0;
	int capacity_ = 0, shown_ = 0;
	bool dirty_ = false;
	void write(const Mark &m, int slot);
};

} // namespace city
