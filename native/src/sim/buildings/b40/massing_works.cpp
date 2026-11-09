// Industrial works made to measure, of four kinds drawn by the weights of buildings.json shapes.works: a metal shed (the style's
// panels, any roof), a brick works (brick walls, a sawtooth or gable roof and a round brick stack), a warehouse (pale panel walls,
// a flat or shed roof) and a shed with a row of round silos. Each a hall of one tall storey with ribbon windows and loading doors
// at the back, and an office annex in front; the stack or the silos stand in a strip beside the hall.
#include "sim/buildings/b40/massing.h"

#include "core/math.h"

#include <cmath>

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f; // the hall's, the annex's, the silos' and the stack's sizes are buildings.json shapes.works, with their sources

enum WorksKind { WORKS_SHED, WORKS_BRICK, WORKS_STORE, WORKS_SILOS, WORKS_KINDS };

WorksKind works_kind(const WorksShape &K, Xoshiro128 &r) {
	const float weights[WORKS_KINDS] = { K.shed_weight, K.brick_weight, K.store_weight, K.silo_weight };
	float total = 0;
	for (float w : weights) total += w;
	float at = float(r.f()) * total;
	for (int k = 0; k < WORKS_KINDS; k++)
		if ((at -= weights[k]) < 0) return WorksKind(k);
	return WORKS_SHED;
}

// A round plan, normalised to its block (the round tower's sides, buildings.json shapes.tall round_sides).
std::vector<V2> round_plan(const Data &data) {
	const int sides = data.shapes.tall.round_sides;
	std::vector<V2> out;
	for (int k = 0; k < sides; k++) {
		const float a = -PI_F / 2 + float(k) / float(sides) * PI_F * 2;
		out.push_back({ HALF + HALF * std::cos(a), HALF + HALF * std::sin(a) });
	}
	return out;
}

// A round block of `storeys` standing in the strip, its walls one paint, not composed (a silo, a stack).
void round_block(const Data &data, Body &b, float x, float y, float side, int storeys, const Paint &paint) {
	const std::vector<V2> plan = round_plan(data);
	BlockSpec s{ x, y, side, side, 0, storeys, ROOF_FLAT, BAY_WALL, BAY_WALL };
	s.shape = &plan;
	Volume &v = b.volumes[size_t(add_block(b, s))];
	v.keep = true;
	v.lines = LINES_NONE;
	v.crown = CROWN_NONE;
	v.wall = b.add_paint(paint);
}
} // namespace

void works(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r) {
	const WorksShape &K = data.shapes.works;
	b.style = STYLE_INDUSTRIAL;
	const WorksKind kind = works_kind(K, r);
	Paint wall, trim;
	paints_of(data, b.style, r, wall, trim);
	if (kind == WORKS_BRICK) wall = choose(r, data.bricks);
	if (kind == WORKS_STORE) wall = Paint{ FIN_PANEL, choose(r, data.warehouse_walls) };
	float hall_h = mixr(r, K.hall_lo, K.hall_hi);
	b.palette = r.irange(0, data.last_palette());
	b.ground_h = hall_h;
	b.storey_h = K.office_storey;
	look(b, wall, choose(r, data.works_roofs), trim);
	b.ground = GROUND_WORKS;
	b.use = USE_INDUSTRIAL;
	float w = snap(env.W), d = snap(env.D);
	// A brick works under the sawtooth of the old mills or a gable; a warehouse flat or a lean-to; a shed any of them.
	Roof roof = kind == WORKS_BRICK ? choose(r, { ROOF_SAWTOOTH, ROOF_SAWTOOTH, ROOF_GABLE })
			: kind == WORKS_STORE ? choose(r, { ROOF_FLAT, ROOF_FLAT, ROOF_SHED })
			: choose(r, { ROOF_SAWTOOTH, ROOF_SHED, ROOF_FLAT, ROOF_GABLE, ROOF_SAWTOOTH });
	bool annex = d >= K.annex_d && w >= K.annex_w;
	float aw = annex ? snap(std::min(w - K.annex_room, mixr(r, K.annex_w_lo, K.annex_w_hi))) : 0;
	float ad = annex ? snap(mixr(r, K.annex_d_lo, K.annex_d_hi)) : 0;
	bool left = r.f() < HALF;
	// The strip beside the hall, on the side away from the annex, for the stack or the silos: only where the hall keeps its width.
	const float feature = kind == WORKS_SILOS ? snap(mixr(r, K.silo_d_lo, K.silo_d_hi)) : kind == WORKS_BRICK ? K.stack_d : 0;
	const float strip = feature > 0 ? feature + 2 * K.feature_gap : 0;
	const bool beside = strip > 0 && w - strip >= K.annex_w;
	const float hall_x = beside && !left ? strip : 0, hall_w = beside ? w - strip : w;
	add_block(b, { hall_x, ad, hall_w, d - ad, 0, 1, roof, BAY_RIBBON, BAY_WALL });
	if (annex) {
		int storeys = r.irange(1, 2);
		std::vector<Paint> hard = data.hard;
		hard.insert(hard.end(), data.bricks.begin(), data.bricks.end());
		int av = add_block(b, { left ? 0 : w - aw, 0, aw, ad, 0, storeys, ROOF_FLAT, BAY_RIBBON, BAY_RIBBON });
		b.volumes[size_t(av)].wall = b.add_paint(choose(r, hard));
	}
	if (beside) {
		const float x = left ? w - strip + K.feature_gap : K.feature_gap;
		if (kind == WORKS_SILOS) {
			const int fits = int(std::floor((d - ad) / (feature + K.feature_gap)));
			const int silos = std::min(fits, r.irange(K.silo_lo, K.silo_hi));
			const Paint metal{ FIN_METAL, choose(r, data.tanks) };
			const int storeys = r.irange(K.silo_storeys_lo, K.silo_storeys_hi);
			for (int k = 0; k < silos; k++) round_block(data, b, x, d - float(k + 1) * (feature + K.feature_gap), feature, storeys, metal);
		} else {
			round_block(data, b, x, d - feature - K.feature_gap, feature, K.stack_storeys, wall);
		}
	}
	roof_shape(data, b.volumes[0], r, 0);
	Volume &hall = b.volumes[0];
	hall.keep = true;
	hall.lines = LINES_NONE;
	hall.crown = CROWN_ATTIC;
	hall.faces[2].bays = bays_of(hall, 2);
	for (int i = 0; i < hall.faces[2].bays; i += K.loading_every) hall.bays[bay_key(0, 2, i)] = BAY_LOADING_DOOR;
	if (annex) {
		Volume &o = b.volumes[1];
		o.keep = true;
		o.lines = LINES_SLAB;
		o.crown = CROWN_SLAB;
	}
	Envelope e = env;
	e.W = annex ? (left ? aw / 2 : w - aw / 2) * 2 : hall_x * 2 + hall_w;
	finish_body(data, b, e, r, BAY_DOOR, annex ? 1 : 0);
}

} // namespace city::b40
