// What reaches a body after it fell (body_renderer.h): a later blow on the same body (one body a casualty, as the reference's ragdoll.ts
// spawn finds the known body first), a death where it lies, its walker's id passing to the dead body's (ped_dead.cpp), and its letting go.
#include "render/agents/body_renderer.h"

#include <algorithm>

using namespace godot;

namespace city {

// A round in somebody lying wounds the body lying there; a blast throws it again.
bool BodyRenderer::strike(const BodySpec &spec, const std::vector<BodySpec> &pieces) {
	if (spec.who < 0) return false;
	const auto found = std::find_if(bodies_.begin(), bodies_.end(), [&](const Body &o) { return !o.piece && o.who == spec.who; });
	if (found == bodies_.end()) return false;
	Body &b = *found;
	b.lost |= spec.lost;
	b.bleeds |= spec.bleeds;
	b.charred = b.charred || spec.charred;
	if (spec.pool > b.pool) { // a graver wound: its pool where the body comes to rest again
		b.pool = spec.pool;
		b.pooled = false;
	}
	if (spec.rises) { // knocked again: down till the simulation's end of it (traffic::lay_down keeps the later end)
		b.left = spec.life_s > 0.f ? spec.life_s : FOR_GOOD_S;
		b.rise_s = spec.life_s > 0.f ? std::min(GET_UP_S, spec.life_s * RISE_SHARE_MOST) : 0.f;
	} else { // killed where they lay, or dead already: a dead body, kept (its id the simulation's to rename: BODY_DEAD)
		b.rise_s = 0.f;
		b.left = FOR_GOOD_S;
	}
	std::vector<Body> torn; // each limb that comes off: a piece of this body as it lies, on a ragdoll of its own
	torn.reserve(pieces.size());
	for (const BodySpec &p : pieces) {
		Body &c = torn.emplace_back(b);
		c.doll.keep_parts(p.piece);
		c.doll.shove(p.velocity, p.scatter, rng_);
		c.piece = true;
		c.root_bone = root_bone_of(p.piece); // its root follows its own part, not the torso it left
		c.lost = p.lost;
		c.who = -1;
		c.pool = 0.f;
		c.pooled = false;
		c.rise_s = 0.f;
		c.left = FOR_GOOD_S;
		c.splashes = 0;
	}
	b.doll.shove(spec.velocity, spec.scatter, rng_);
	pose(b);
	const uint32_t family = b.family; // b is touched no more: a row taken may erase a body in the middle, which moves the others
	for (Body &c : torn) {
		c.slot = take_slot(family);
		pose(bodies_.emplace_back(std::move(c)));
	}
	layers_dirty_ = true;
	return true;
}

void BodyRenderer::die(int who) {
	if (who < 0) return;
	for (Body &b : bodies_) {
		if (b.who != who) continue;
		b.rise_s = 0.f;
		b.left = FOR_GOOD_S;
	}
}

void BodyRenderer::rename(int from, int to) {
	if (from < 0) return;
	for (Body &b : bodies_)
		if (!b.piece && b.who == from) b.who = to;
}

void BodyRenderer::let_go(int who) {
	if (who < 0) return;
	const auto found = std::find_if(bodies_.begin(), bodies_.end(), [&](const Body &o) { return !o.piece && o.who == who; });
	if (found == bodies_.end()) return;
	const uint32_t family = found->family;
	const auto gone = std::remove_if(bodies_.begin(), bodies_.end(), [&](const Body &o) {
		if (o.family != family) return false;
		free_slots_.push_back(o.slot);
		return true;
	});
	bodies_.erase(gone, bodies_.end());
	layers_dirty_ = true;
}

} // namespace city
