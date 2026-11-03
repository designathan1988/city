// Industrial halls made to measure (jobs/b40/massing.ts works; dormant in R5, drawn here for industrial lots instead of
// the legacy warehouse): a hall of one tall storey with ribbon windows and loading doors at the back, a sawtooth,
// shed, flat or gable roof, and an office annex in front.
#include "render/buildings/massing.h"

namespace city::b40 {

namespace {
constexpr int PALETTES = 7;
constexpr float HALL_LO = 7.f, HALL_HI = 9.f, OFFICE_STOREY = 3.4f, ANNEX_D = 20.f, ANNEX_W = 12.f, ANNEX_ROOM = 4.f;
constexpr float ANNEX_W_LO = 6.f, ANNEX_W_HI = 9.f, ANNEX_D_LO = 5.f, ANNEX_D_HI = 7.f, HALF = 0.5f;
constexpr int LOADING_EVERY = 2;
} // namespace

void works(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r) {
	b.style = STYLE_INDUSTRIAL;
	Paint wall, trim;
	paints_of(data, b.style, r, wall, trim);
	float hall_h = mixr(r, HALL_LO, HALL_HI);
	b.palette = r.irange(0, PALETTES);
	b.ground_h = hall_h;
	b.storey_h = OFFICE_STOREY;
	look(b, wall, choose(r, data.works_roofs), trim);
	b.ground = GROUND_WORKS;
	b.use = USE_INDUSTRIAL;
	float w = snap(env.W), d = snap(env.D);
	Roof roof = choose(r, { ROOF_SAWTOOTH, ROOF_SHED, ROOF_FLAT, ROOF_GABLE, ROOF_SAWTOOTH });
	bool annex = d >= ANNEX_D && w >= ANNEX_W;
	float aw = annex ? snap(std::min(w - ANNEX_ROOM, mixr(r, ANNEX_W_LO, ANNEX_W_HI))) : 0;
	float ad = annex ? snap(mixr(r, ANNEX_D_LO, ANNEX_D_HI)) : 0;
	bool left = r.f() < HALF;
	add_block(b, { 0, ad, w, d - ad, 0, 1, roof, BAY_RIBBON, BAY_WALL });
	if (annex) {
		int storeys = r.irange(1, 2);
		std::vector<Paint> hard = data.hard;
		hard.insert(hard.end(), data.bricks.begin(), data.bricks.end());
		int av = add_block(b, { left ? 0 : w - aw, 0, aw, ad, 0, storeys, ROOF_FLAT, BAY_RIBBON, BAY_RIBBON });
		b.volumes[size_t(av)].wall = b.add_paint(choose(r, hard));
	}
	roof_shape(b.volumes[0], r, 0);
	Volume &hall = b.volumes[0];
	hall.keep = true;
	hall.lines = LINES_NONE;
	hall.crown = CROWN_ATTIC;
	hall.faces[2].bays = bays_of(hall, 2);
	for (int i = 0; i < hall.faces[2].bays; i += LOADING_EVERY) hall.bays[bay_key(0, 2, i)] = BAY_LOADING_DOOR;
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
