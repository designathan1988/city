// Service buildings (in the envelope frame: x from its left edge, y from its front): gas station
// (shop at the back, a 5.5 m canopy on columns over the pumps and an asphalt apron), supermarket / mall (a 7 / 14 m
// box with a glazed shopfront, a sign band and parking in front), city hall (13 m stone block with a six-column
// portico and stepped pediment) and police / fire station (8 m, garage doors, the fire station's 18 m hose tower).
#include "sim/buildings/b40/civic.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f;

float env_w(const Body &b) { return b.plan.x1 - b.plan.x0; }
float env_d(const Body &b) { return b.plan.y1 - b.plan.y0; }

void flat_block(Body &b, int i, Lines lines, Crown crown) {
	Volume &v = b.volumes[size_t(i)];
	v.keep = true;
	v.lines = lines;
	v.crown = crown;
}
} // namespace

void gas_station(const Data &data, Body &b, Xoshiro128 &r) {
	const ServiceShape &K = data.shapes.services;
	const LotPartShape &P = data.shapes.parts;
	float w = env_w(b), d = env_d(b);
	b.ground_h = K.shop_h;
	float sw = std::min(K.store_front_w, w), sd = std::min(K.shop_d, d * HALF);
	bool left = r.f() < HALF;
	flat_block(b, add_block(b, { left ? 0 : w - sw, d - sd, sw, sd, 0, 1, ROOF_FLAT, BAY_WINDOW, BAY_SHOPFRONT }), LINES_NONE, CROWN_SLAB);
	put(b, EL_SURFACE, 0, 0, w, d - sd, P.cover_h, b.add_paint(data.color("asphalt")));
	float cw = std::min(K.canopy_w_max, w - 2 * K.canopy_edge), cd = std::min(K.canopy_d, d - sd - K.canopy_edge), cx = w / 2, cy = (d - sd) / 2;
	if (cw <= 0 || cd <= 0) return;
	int canopy = b.add_paint(data.color("fuel_canopy")), stripe = b.add_paint(data.color("fuel_brand"));
	put_at(b, EL_CANOPY, cx, cy, cw, cd, K.canopy_t, canopy, K.canopy_z);
	put_at(b, EL_CANOPY, cx, cy - cd / 2, cw, K.stripe_t, K.canopy_t * HALF, stripe, K.canopy_z + K.canopy_t * HALF * HALF);
	int col = b.add_paint(data.color("column")), pump = b.add_paint(data.color("fuel_pump")), island = b.add_paint(data.color("concrete"));
	for (float x = cx - cw / 2 + K.pump_every / 2; x < cx + cw / 2; x += K.pump_every) {
		put_at(b, EL_PILLAR, x, cy, K.column, K.column, K.canopy_z, col);
		put_at(b, EL_SLAB, x, cy, K.island_w, K.island_d, K.island_h, island);
		for (float s : { -1.f, 1.f }) put_at(b, EL_BIN, x, cy + s * (K.island_d / 2 - K.pump_d), K.pump_w, K.pump_d, K.pump_h, pump);
	}
}

void market(const Data &data, Body &b, Xoshiro128 &r, bool mall) {
	const ServiceShape &K = data.shapes.services;
	const LotPartShape &P = data.shapes.parts;
	float w = env_w(b), d = env_d(b);
	float park = std::min(K.park_max, d * K.park_share);
	b.ground_h = K.market_h;
	b.storey_h = K.mall_h;
	int box = add_block(b, { 0, park, w, d - park, 0, mall ? 2 : 1, ROOF_FLAT, BAY_WALL, BAY_SHOPFRONT });
	flat_block(b, box, LINES_NONE, CROWN_ATTIC);
	int asphalt = b.add_paint(data.color("asphalt")), line = b.add_paint(data.color("stall_line"));
	put(b, EL_SURFACE, 0, 0, w, park, P.cover_h, asphalt);
	for (float y0 : { 0.f, K.stall_d + K.aisle })
		if (y0 + K.stall_d <= park)
			for (float x = data.lots.stall_w; x < w; x += data.lots.stall_w) put(b, EL_SURFACE, x - K.stripe_w / 2, y0, x + K.stripe_w / 2, y0 + K.stall_d, P.cover_h * 2, line);
	float top = b.level_z(mall ? 2 : 1);
	put_at(b, EL_CANOPY, w / 2, park - K.sign_out, w * K.sign_share, K.sign_out, K.sign_h, b.add_paint(data.color(mall ? "mall_sign" : "market_sign")), top - K.sign_h * 2);
	put_at(b, EL_CANOPY, r.f() < HALF ? K.cart_w : w - K.cart_w, park - K.cart_d, K.cart_w, K.cart_d, K.cart_h, b.add_paint(data.color("plant_metal")), 0);
}

void city_hall(const Data &data, Body &b, Xoshiro128 &) {
	const ServiceShape &K = data.shapes.services;
	float w = env_w(b), d = env_d(b);
	b.ground_h = K.hall_ground;
	b.storey_h = K.hall_storey;
	int main = add_block(b, { 0, K.portico_d, w, d - K.portico_d, 0, K.hall_storeys, ROOF_HIP, BAY_SASH, BAY_SASH });
	flat_block(b, main, LINES_BASE, CROWN_DECO);
	b.volumes[size_t(main)].roof_paint = b.add_paint(data.color("hall_roof"));
	int stone = b.add_paint(data.color("civic_stone")), col = b.add_paint(data.color("column"));
	float pw = std::min(K.portico_w_max, w * K.portico_w_share), x0 = w / 2 - pw / 2;
	put(b, EL_SLAB, x0 - K.step_in, -K.stair_d, x0 + pw + K.step_in, K.portico_d, K.stair_h, stone);
	for (int i = 0; i < K.columns; i++) {
		float x = x0 + K.col / 2 + (pw - K.col) * float(i) / float(K.columns - 1);
		put_at(b, EL_PILLAR, x, K.col, K.col, K.col, K.col_h, col);
	}
	put(b, EL_CANOPY, x0, 0, x0 + pw, K.portico_d, K.entab_h, stone, K.col_h);
	for (int k = 0; k < K.pediment_steps; k++) {
		float in = K.step_in * float(k + 1);
		put(b, EL_CANOPY, x0 + in, 0, x0 + pw - in, K.portico_d, K.step_h, stone, K.col_h + K.entab_h + K.step_h * float(k));
	}
	float roof = b.level_z(K.hall_storeys);
	put_at(b, EL_CANOPY, w / 2, K.portico_d + 1, K.flag_pole, K.flag_pole, K.flag_h, b.add_paint(data.color("plant_dark")), roof);
	put_at(b, EL_CANOPY, w / 2 + K.flag_w / 2, K.portico_d + 1, K.flag_w, K.flag_t, K.flag_d, b.add_paint(data.color("flag")), roof + K.flag_h - K.flag_d);
}

void station(const Data &data, Body &b, Xoshiro128 &r, bool fire) {
	const ServiceShape &K = data.shapes.services;
	const LotPartShape &P = data.shapes.parts;
	float w = env_w(b), d = env_d(b);
	b.ground_h = b.storey_h = K.station_storey;
	int main = add_block(b, { 0, 0, w, d, 0, K.station_storeys, ROOF_FLAT, BAY_WINDOW, fire ? BAY_GARAGE_DOOR : BAY_WINDOW });
	flat_block(b, main, LINES_SLAB, CROWN_CORNICE);
	Volume &v = b.volumes[size_t(main)];
	if (fire) {
		v.faces[0].bays = K.garage_bays + 1;
		bool left = r.f() < HALF;
		int tower = add_block(b, { left ? 0 : w - K.tower_w, d - K.tower_w, K.tower_w, K.tower_w, K.station_storeys, K.tower_storeys, ROOF_FLAT, BAY_WINDOW, BAY_NONE });
		flat_block(b, tower, LINES_EVERY, CROWN_CORNICE);
		b.volumes[size_t(tower)].wall = b.add_paint(data.color("fire_tower"));
		put(b, EL_SURFACE, 0, -b.plan.front_depth, w, 0, P.cover_h * 2, b.add_paint(data.color("concrete")));
	}
	int band = b.add_paint(data.color(fire ? "fire_band" : "police_band"));
	put(b, EL_CANOPY, -K.band_out, -K.band_out, w + K.band_out, 0, K.band_h, band, b.level_z(K.station_storeys) - K.band_h * 2);
}

} // namespace city::b40
