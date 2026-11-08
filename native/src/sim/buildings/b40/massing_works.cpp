// Industrial halls made to measure: a hall of one tall storey with ribbon windows and loading doors at the back, a sawtooth,
// shed, flat or gable roof, and an office annex in front.
#include "sim/buildings/b40/massing.h"

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f; // the hall's and the annex's sizes are buildings.json shapes.works, with their sources
} // namespace

void works(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r) {
	const WorksShape &K = data.shapes.works;
	b.style = STYLE_INDUSTRIAL;
	Paint wall, trim;
	paints_of(data, b.style, r, wall, trim);
	float hall_h = mixr(r, K.hall_lo, K.hall_hi);
	b.palette = r.irange(0, data.last_palette());
	b.ground_h = hall_h;
	b.storey_h = K.office_storey;
	look(b, wall, choose(r, data.works_roofs), trim);
	b.ground = GROUND_WORKS;
	b.use = USE_INDUSTRIAL;
	float w = snap(env.W), d = snap(env.D);
	Roof roof = choose(r, { ROOF_SAWTOOTH, ROOF_SHED, ROOF_FLAT, ROOF_GABLE, ROOF_SAWTOOTH });
	bool annex = d >= K.annex_d && w >= K.annex_w;
	float aw = annex ? snap(std::min(w - K.annex_room, mixr(r, K.annex_w_lo, K.annex_w_hi))) : 0;
	float ad = annex ? snap(mixr(r, K.annex_d_lo, K.annex_d_hi)) : 0;
	bool left = r.f() < HALF;
	add_block(b, { 0, ad, w, d - ad, 0, 1, roof, BAY_RIBBON, BAY_WALL });
	if (annex) {
		int storeys = r.irange(1, 2);
		std::vector<Paint> hard = data.hard;
		hard.insert(hard.end(), data.bricks.begin(), data.bricks.end());
		int av = add_block(b, { left ? 0 : w - aw, 0, aw, ad, 0, storeys, ROOF_FLAT, BAY_RIBBON, BAY_RIBBON });
		b.volumes[size_t(av)].wall = b.add_paint(choose(r, hard));
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
	e.W = annex ? (left ? aw / 2 : w - aw / 2) * 2 : env.W;
	finish_body(data, b, e, r, BAY_DOOR, annex ? 1 : 0);
}

} // namespace city::b40
