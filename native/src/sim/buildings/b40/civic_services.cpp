// Service buildings (in the envelope frame: x from its left edge, y from its front): gas station
// (shop at the back, a 5.5 m canopy on columns over the pumps and an asphalt apron), supermarket / mall (a 7 / 14 m
// box with a glazed shopfront, a sign band and parking in front), city hall (13 m stone block with a six-column
// portico and stepped pediment) and police / fire station (8 m, garage doors, the fire station's 18 m hose tower).
#include "sim/buildings/b40/civic.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f, SURFACE_H = 0.06f;
// gas station
constexpr float STORE_FRONT_W = 10.f, SHOP_D = 8.f, SHOP_H = 4.f, CANOPY_Z = 5.5f, CANOPY_T = 0.7f, CANOPY_W_MAX = 18.f, CANOPY_D = 10.f;
constexpr float CANOPY_EDGE = 1.f, COLUMN = 0.45f, PUMP_W = 0.7f, PUMP_D = 1.2f, PUMP_H = 1.8f, ISLAND_W = 1.2f, ISLAND_D = 4.f;
constexpr float ISLAND_H = 0.18f, PUMP_EVERY = 6.f, STRIPE_T = 0.25f;
// market
constexpr float MARKET_H = 7.f, MALL_H = 7.f, SIGN_H = 1.4f, SIGN_OUT = 0.25f, SIGN_SHARE = 0.5f, STALL_W = 2.5f, STALL_D = 5.f;
constexpr float STRIPE_W = 0.12f, AISLE = 6.f, PARK_SHARE = 0.42f, PARK_MAX = 22.f, CART_W = 2.5f, CART_D = 1.2f, CART_H = 1.1f;
// city hall
constexpr float HALL_GROUND = 5.f, HALL_STOREY = 4.f, PORTICO_D = 4.5f, PORTICO_W_SHARE = 0.55f, PORTICO_W_MAX = 20.f, COL = 0.9f;
constexpr float COL_H = 9.f, ENTAB_H = 1.2f, STEP_H = 0.7f, STEP_IN = 1.6f, FLAG_POLE = 0.12f, FLAG_H = 7.f, FLAG_W = 1.8f;
constexpr float FLAG_D = 1.1f, FLAG_T = 0.05f, STAIR_H = 0.5f, STAIR_D = 1.5f;
constexpr int COLUMNS = 6, HALL_STOREYS = 3, PEDIMENT_STEPS = 3;
// stations
constexpr float STATION_STOREY = 4.f, TOWER_W = 4.f, BAND_H = 0.6f, BAND_OUT = 0.15f;
constexpr int STATION_STOREYS = 2, TOWER_STOREYS = 3, GARAGE_BAYS = 3; // tower: 2 + 3 storeys of 4 m = 20 m

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
	float w = env_w(b), d = env_d(b);
	b.ground_h = SHOP_H;
	float sw = std::min(STORE_FRONT_W, w), sd = std::min(SHOP_D, d * HALF);
	bool left = r.f() < HALF;
	flat_block(b, add_block(b, { left ? 0 : w - sw, d - sd, sw, sd, 0, 1, ROOF_FLAT, BAY_WINDOW, BAY_SHOPFRONT }), LINES_NONE, CROWN_SLAB);
	put(b, EL_SURFACE, 0, 0, w, d - sd, SURFACE_H, b.add_paint(data.color("asphalt")));
	float cw = std::min(CANOPY_W_MAX, w - 2 * CANOPY_EDGE), cd = std::min(CANOPY_D, d - sd - CANOPY_EDGE), cx = w / 2, cy = (d - sd) / 2;
	if (cw <= 0 || cd <= 0) return;
	int canopy = b.add_paint(data.color("fuel_canopy")), stripe = b.add_paint(data.color("fuel_brand"));
	put_at(b, EL_CANOPY, cx, cy, cw, cd, CANOPY_T, canopy, CANOPY_Z);
	put_at(b, EL_CANOPY, cx, cy - cd / 2, cw, STRIPE_T, CANOPY_T * HALF, stripe, CANOPY_Z + CANOPY_T * HALF * HALF);
	int col = b.add_paint(data.color("column")), pump = b.add_paint(data.color("fuel_pump")), island = b.add_paint(data.color("concrete"));
	for (float x = cx - cw / 2 + PUMP_EVERY / 2; x < cx + cw / 2; x += PUMP_EVERY) {
		put_at(b, EL_PILLAR, x, cy, COLUMN, COLUMN, CANOPY_Z, col);
		put_at(b, EL_SLAB, x, cy, ISLAND_W, ISLAND_D, ISLAND_H, island);
		for (float s : { -1.f, 1.f }) put_at(b, EL_BIN, x, cy + s * (ISLAND_D / 2 - PUMP_D), PUMP_W, PUMP_D, PUMP_H, pump);
	}
}

void market(const Data &data, Body &b, Xoshiro128 &r, bool mall) {
	float w = env_w(b), d = env_d(b);
	float park = std::min(PARK_MAX, d * PARK_SHARE);
	b.ground_h = MARKET_H;
	b.storey_h = MALL_H;
	int box = add_block(b, { 0, park, w, d - park, 0, mall ? 2 : 1, ROOF_FLAT, BAY_WALL, BAY_SHOPFRONT });
	flat_block(b, box, LINES_NONE, CROWN_ATTIC);
	int asphalt = b.add_paint(data.color("asphalt")), line = b.add_paint(data.color("stall_line"));
	put(b, EL_SURFACE, 0, 0, w, park, SURFACE_H, asphalt);
	for (float y0 : { 0.f, STALL_D + AISLE })
		if (y0 + STALL_D <= park)
			for (float x = STALL_W; x < w; x += STALL_W) put(b, EL_SURFACE, x - STRIPE_W / 2, y0, x + STRIPE_W / 2, y0 + STALL_D, SURFACE_H * 2, line);
	float top = b.level_z(mall ? 2 : 1);
	put_at(b, EL_CANOPY, w / 2, park - SIGN_OUT, w * SIGN_SHARE, SIGN_OUT, SIGN_H, b.add_paint(data.color(mall ? "mall_sign" : "market_sign")), top - SIGN_H * 2);
	put_at(b, EL_CANOPY, r.f() < HALF ? CART_W : w - CART_W, park - CART_D, CART_W, CART_D, CART_H, b.add_paint(data.color("plant_metal")), 0);
}

void city_hall(const Data &data, Body &b, Xoshiro128 &) {
	float w = env_w(b), d = env_d(b);
	b.ground_h = HALL_GROUND;
	b.storey_h = HALL_STOREY;
	int main = add_block(b, { 0, PORTICO_D, w, d - PORTICO_D, 0, HALL_STOREYS, ROOF_HIP, BAY_SASH, BAY_SASH });
	flat_block(b, main, LINES_BASE, CROWN_DECO);
	b.volumes[size_t(main)].roof_paint = b.add_paint(data.color("hall_roof"));
	int stone = b.add_paint(data.color("civic_stone")), col = b.add_paint(data.color("column"));
	float pw = std::min(PORTICO_W_MAX, w * PORTICO_W_SHARE), x0 = w / 2 - pw / 2;
	put(b, EL_SLAB, x0 - STEP_IN, -STAIR_D, x0 + pw + STEP_IN, PORTICO_D, STAIR_H, stone);
	for (int i = 0; i < COLUMNS; i++) {
		float x = x0 + COL / 2 + (pw - COL) * float(i) / float(COLUMNS - 1);
		put_at(b, EL_PILLAR, x, COL, COL, COL, COL_H, col);
	}
	put(b, EL_CANOPY, x0, 0, x0 + pw, PORTICO_D, ENTAB_H, stone, COL_H);
	for (int k = 0; k < PEDIMENT_STEPS; k++) {
		float in = STEP_IN * float(k + 1);
		put(b, EL_CANOPY, x0 + in, 0, x0 + pw - in, PORTICO_D, STEP_H, stone, COL_H + ENTAB_H + STEP_H * float(k));
	}
	float roof = b.level_z(HALL_STOREYS);
	put_at(b, EL_CANOPY, w / 2, PORTICO_D + 1, FLAG_POLE, FLAG_POLE, FLAG_H, b.add_paint(data.color("plant_dark")), roof);
	put_at(b, EL_CANOPY, w / 2 + FLAG_W / 2, PORTICO_D + 1, FLAG_W, FLAG_T, FLAG_D, b.add_paint(data.color("flag")), roof + FLAG_H - FLAG_D);
}

void station(const Data &data, Body &b, Xoshiro128 &r, bool fire) {
	float w = env_w(b), d = env_d(b);
	b.ground_h = b.storey_h = STATION_STOREY;
	int main = add_block(b, { 0, 0, w, d, 0, STATION_STOREYS, ROOF_FLAT, BAY_WINDOW, fire ? BAY_GARAGE_DOOR : BAY_WINDOW });
	flat_block(b, main, LINES_SLAB, CROWN_CORNICE);
	Volume &v = b.volumes[size_t(main)];
	if (fire) {
		v.faces[0].bays = GARAGE_BAYS + 1;
		bool left = r.f() < HALF;
		int tower = add_block(b, { left ? 0 : w - TOWER_W, d - TOWER_W, TOWER_W, TOWER_W, STATION_STOREYS, TOWER_STOREYS, ROOF_FLAT, BAY_WINDOW, BAY_NONE });
		flat_block(b, tower, LINES_EVERY, CROWN_CORNICE);
		b.volumes[size_t(tower)].wall = b.add_paint(data.color("fire_tower"));
		put(b, EL_SURFACE, 0, -b.plan.front_depth, w, 0, SURFACE_H * 2, b.add_paint(data.color("concrete")));
	}
	int band = b.add_paint(data.color(fire ? "fire_band" : "police_band"));
	put(b, EL_CANOPY, -BAND_OUT, -BAND_OUT, w + BAND_OUT, 0, BAND_H, band, b.level_z(STATION_STOREYS) - BAND_H * 2);
}

} // namespace city::b40
