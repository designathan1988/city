// What a blast leaves on the ground, flat quads just over it in one MultiMesh (render/shaders/marks.gdshader draws each
// from its kind): the craters of the damage state (drawn again whenever the state changes: undo, redo, open; a blow only adds
// its own), soot and blood kept (the oldest go first), and the shock rings while they race out. Every one of them is drawn: the
// layer holds what these lists hold (the state's craters, soot_kept and blood_kept), it keeps no count of its own.
#pragma once

#include "render/effects/mm_layer.h"
#include "core/shader_contracts/effects.h"
#include "sim/demolition/damage_state.h"

#include <algorithm>
#include <deque>
#include <vector>

namespace city {

// Mark kinds (MarkKind) are core/shader_contracts/effects.h's, which marks.gdshader reads.

struct Mark {
	float x = 0, y = 0, z = 0, size = 0, seed = 0;
	int kind = 0;
	double born = 0, life = 0;
};

class Marks {
public:
	// A crater's quad is this many times its hole across: the scorch and the broken lip round it (the ground it heaved is laid out to
	// the same reach, heave.h).
	void init(const godot::RID &scenario, const godot::RID &material, int room, int soot_kept, int blood_kept); // room: the layer's first size
	void reset();
	void release();
	int blood() const { return int(blood_.size()); }
	// The craters as the state has them (`ground_of`: the terrain's natural height at a point). `kept`: how many of the first are
	// those already laid (the list only grew past them since): of those, only the ones whose ground moved since are laid again.
	template <typename F>
	void set_craters(const std::vector<Crater> &list, size_t kept, F &&ground_of) {
		kept = std::min({ kept, craters_.size(), list.size() });
		craters_.resize(kept);
		stale_.resize(kept);
		for (size_t i = 0; i < kept; i++)
			if (stale_[i]) {
				craters_[i] = crater_mark(list[i], ground_of);
				stale_[i] = 0;
			}
		for (size_t i = kept; i < list.size(); i++) {
			craters_.push_back(crater_mark(list[i], ground_of));
			stale_.push_back(0);
		}
		dirty_ = true;
	}
	// The natural ground moved where `moved(x, z, half)` says (a crater's decal, `half` metres out each way, reads it there): those
	// craters are laid again the next time the list changes (set_craters), as every crater was before.
	template <typename T>
	void ground_moved(T &&moved) {
		for (size_t i = 0; i < craters_.size(); i++)
			if (moved(craters_[i].x, craters_[i].z, craters_[i].size * 0.5f)) stale_[i] = 1;
	}
	template <typename F>
	static Mark crater_mark(const Crater &c, F &&ground_of) {
		Mark m;
		m.kind = c.hard ? MARK_CRATER_HARD : MARK_CRATER_EARTH;
		m.x = c.x; m.z = c.z; m.size = c.radius * 2.f * CRATER_SCALE; m.seed = float(c.seed & SEED_MASK) / float(SEED_MASK);
		m.y = top_ground(c.x, c.z, m.size * 0.5f, ground_of) + SURFACE_LIFT_M; // the pit the dig leaves lies below it
		if (c.surface_gone(ground_of(c.x, c.z))) m.size = 0.f; // dug in a deck cut away since: nothing of it on the ground under it
		return m;
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
	static constexpr float SURFACE_LIFT_M = 0.06f; // a decal lies this far over the ground, so the two never fight
	static constexpr int RELIEF_GRID = 4; // the ground is sampled on a grid of (2 * this + 1)^2 points across a decal
	static constexpr float CRATER_SCALE = 2.4f; // the quad is wider than the hole: the scorch and the broken lip round it (blast_probe says how far)

private:
	static constexpr uint32_t SEED_MASK = 0xffffu; // a decal's seed: sixteen bits of its hash
	MmLayer layer_;
	godot::RID material_;
	std::vector<Mark> craters_;
	std::vector<uint8_t> stale_; // by crater: its ground moved since it was laid
	std::deque<Mark> soot_, blood_;
	std::vector<Mark> rings_;
	size_t soot_kept_ = 0, blood_kept_ = 0;
	int shown_ = 0;
	bool dirty_ = false;
	void write(const Mark &m, int slot);
};

} // namespace city
