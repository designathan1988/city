// What the tests and look plans read of the fallen (BodyRenderer::census, places): probes, never drawn from.
#include "render/agents/body_renderer.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

PackedVector3Array BodyRenderer::places() const {
	PackedVector3Array out;
	for (const Body &b : bodies_)
		if (!b.piece) out.push_back(b.doll.pelvis()); // a limb torn off has no pelvis: it lies by its body, at its joint
	return out;
}

Dictionary BodyRenderer::census() const {
	int moving = 0, kept = 0, from_drawn = 0, pieces = 0, street_walls = 0, unfinite = 0;
	float overlap = 0.f, in_walls = 0.f, widest_m = 0.f;
	PackedInt32Array who;
	Dictionary widest; // the body whose skin spreads widest: a body is no wider than it is tall, a limb torn off less
	for (const Body &b : bodies_) {
		if (!b.piece && b.who >= 0) who.push_back(b.who);
		const float wide = b.box.get_longest_axis_size();
		if (!std::isfinite(wide) || !b.box.position.is_finite()) unfinite++;
		else if (wide > widest_m) {
			widest_m = wide;
			widest["who"] = b.who;
			widest["piece"] = b.piece;
			widest["lost"] = int(b.lost);
			widest["rising"] = b.rise_s > 0.f && b.left <= b.rise_s;
			widest["asleep"] = b.doll.asleep();
			widest["at"] = b.box.get_center();
		}
		moving += b.doll.asleep() ? 0 : 1;
		kept += b.left == FOR_GOOD_S ? 1 : 0;
		from_drawn += b.from_drawn ? 1 : 0;
		pieces += b.piece ? 1 : 0;
		street_walls += b.doll.footed_walls();
		if (!b.doll.asleep()) continue;
		in_walls = std::max(in_walls, b.doll.in_walls());
		for (const Body &o : bodies_)
			if (o.family != b.family && o.doll.asleep() && o.doll.pelvis().distance_squared_to(b.doll.pelvis()) < NEAR_BODY_M * NEAR_BODY_M)
				overlap = std::max(overlap, b.doll.overlap(o.doll));
	}
	Dictionary out;
	out["bodies"] = int(bodies_.size());
	out["moving"] = moving;
	out["kept"] = kept;
	out["from_drawn"] = from_drawn;
	out["pieces"] = pieces;
	out["deepest"] = deepest_;
	out["overlap"] = overlap;
	out["in_walls"] = in_walls;
	out["street_walls"] = street_walls;
	out["who"] = who;
	out["widest_m"] = widest_m;
	out["widest"] = widest;
	out["unfinite"] = unfinite;
	return out;
}

} // namespace city
