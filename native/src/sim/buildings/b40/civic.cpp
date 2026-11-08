// Civic bodies (drawn with the facade grammar of facade_compose.cpp): a plaza in front, the
// building in an envelope set back from the street, then the function's own massing. School: an L of classrooms (2-3
// storeys x 3.6 m, cream, blue-grey hip roof) round a lawn court with a sports court. Church: a nave 9 m high under a
// steep clay gable with a 20 m bell tower, hip spire and gold cross in front. Hospital / clinic: a white ribbon-window
// block (5-9 / 2 storeys) on a glazed podium, blue band, red cross panel and an entrance canopy.
#include "sim/buildings/b40/civic.h"

#include "core/math.h"
#include "core/units.h"
#include "sim/buildings/building_store.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f;

bool is_service(int fn) {
	return fn == FN_GAS_STATION || fn == FN_SUPERMARKET || fn == FN_MALL || fn == FN_CITY_HALL || fn == FN_POLICE || fn == FN_FIRE_STATION;
}

void school(const Data &data, Body &b, float w, float d, Xoshiro128 &r) {
	const CivicShape &K = data.shapes.civic;
	int storeys = r.irange(K.school_storeys_lo, K.school_storeys_hi);
	float wd = std::min(K.wing_d, d * K.wing_d_share), sw = std::min(K.side_wing_w, w * K.side_wing_share);
	bool left = r.f() < HALF;
	int roof = b.add_paint(data.color("school_roof"));
	int a = add_block(b, { 0, 0, w, wd, 0, storeys, ROOF_HIP, BAY_WIDE, BAY_WIDE });
	int c = add_block(b, { left ? 0 : w - sw, wd, sw, d - wd, 0, storeys, ROOF_HIP, BAY_WIDE, BAY_WIDE });
	for (int i : { a, c }) {
		Volume &v = b.volumes[size_t(i)];
		v.pitch = K.school_roof_pitch;
		v.roof_paint = roof;
		v.lines = LINES_BASE;
		v.crown = CROWN_NONE;
		v.keep = true;
	}
	// the court: lawn round a painted sports court on the open side of the L
	float x0 = left ? sw : 0, x1 = left ? w : w - sw;
	put(b, EL_SURFACE, x0, wd, x1, d, K.surface_h, b.add_paint(data.color("lawn")));
	float cw = std::min(K.court_w, x1 - x0 - 2 * K.court_edge), cd = std::min(K.court_d, d - wd - 2 * K.court_edge);
	if (cw <= 0 || cd <= 0) return;
	float cx = (x0 + x1) / 2, cy = (wd + d) / 2;
	put_at(b, EL_SURFACE, cx, cy, cw, cd, K.surface_h * 2, b.add_paint(data.color("court")));
	int line = b.add_paint(data.color("court_line"));
	put_at(b, EL_SURFACE, cx, cy, cw, K.court_line, K.surface_h * 3, line);
	for (float s : { -1.f, 1.f }) {
		put_at(b, EL_SURFACE, cx + s * (cw - K.court_line) / 2, cy, K.court_line, cd, K.surface_h * 3, line);
		put_at(b, EL_SURFACE, cx, cy + s * (cd - K.court_line) / 2, cw, K.court_line, K.surface_h * 3, line);
	}
}

void cross(const CivicShape &K, Body &b, float x, float y, float z, int paint) {
	put_at(b, EL_CANOPY, x, y, K.cross_t, K.cross_t, K.cross_h, paint, z);
	put_at(b, EL_CANOPY, x, y, K.cross_w, K.cross_t, K.cross_t, paint, z + K.cross_h * K.cross_arm_z);
}

int church(const Data &data, Body &b, float w, float d) {
	const CivicShape &K = data.shapes.civic;
	b.ground_h = K.nave_h;
	b.storey_h = K.tower_storey;
	float nw = std::min(K.nave_w, w * K.nave_w_share), nd = std::min(K.nave_d, d - K.tower_w / 2);
	float cx = w / 2;
	int nave = add_block(b, { cx - nw / 2, K.tower_w / 2, nw, nd, 0, 1, ROOF_GABLE, BAY_WINDOW, BAY_WINDOW });
	int tower = add_block(b, { cx - K.tower_w / 2, 0, K.tower_w, K.tower_w, 0, 1 + K.tower_storeys, ROOF_HIP, BAY_WINDOW, BAY_DOUBLE_DOOR });
	int roof = b.add_paint(data.color("church_roof"));
	Volume &n = b.volumes[size_t(nave)];
	n.ridge = 1;
	n.pitch = K.nave_pitch;
	n.roof_paint = roof;
	Volume &t = b.volumes[size_t(tower)];
	t.pitch = K.tower_pitch;
	t.roof_paint = roof;
	for (Volume *v : { &n, &t }) {
		v->keep = true;
		v->lines = LINES_EVERY;
		v->crown = CROWN_NONE;
	}
	float top = b.level_z(t.top()) + K.tower_w / 2 * std::tan(K.tower_pitch * DEG_TO_RAD);
	cross(K, b, cx, K.tower_w / 2, top, b.add_paint(data.color("gold")));
	cross(K, b, cx, K.tower_w / 2 + nd, b.level_z(1) + nw / 2 * std::tan(K.nave_pitch * DEG_TO_RAD), b.add_paint(data.color("gold")));
	return tower;
}

void hospital(const Data &data, Body &b, float w, float d, int floors, bool clinic, Xoshiro128 &r) {
	const CivicShape &K = data.shapes.civic;
	int storeys = clinic ? K.clinic_storeys : std::clamp(floors, K.hospital_lo, K.hospital_hi);
	if (!clinic && floors < K.hospital_lo) storeys = r.irange(K.hospital_lo, K.hospital_hi);
	float ww = w * K.ward_share, wd = std::min(K.ward_d, d * K.ward_d_share), x = (w - ww) / 2;
	int podium = add_block(b, { 0, 0, w, d * K.podium_d_share, 0, 1, ROOF_FLAT, BAY_RIBBON, BAY_SHOPFRONT });
	int ward = add_block(b, { x, 0, ww, wd, 1, storeys - 1, ROOF_FLAT, BAY_RIBBON, BAY_NONE });
	for (int i : { podium, ward }) {
		Volume &v = b.volumes[size_t(i)];
		v.keep = true;
		v.lines = LINES_SLAB;
		v.crown = CROWN_SLAB;
	}
	float top = b.level_z(storeys);
	int band = b.add_paint(data.color("hospital_band")), red = b.add_paint(data.color("red_cross")), white = b.add_paint(data.color("sign_white"));
	put(b, EL_CANOPY, x - K.band_out, -K.band_out, x + ww + K.band_out, 0, K.band_h, band, top - K.band_h * 2);
	put(b, EL_CANOPY, -K.band_out, -K.band_out, w + K.band_out, 0, K.band_h, band, b.level_z(1) - K.band_h);
	float sx = x + ww - K.sign_w, sz = top - K.band_h * 3 - K.sign_w, arm = K.sign_w * K.sign_bar, bar = K.sign_w * (1 - K.sign_bar);
	put_at(b, EL_CANOPY, sx, -K.sign_out / 2, K.sign_w, K.sign_t, K.sign_w, white, sz);
	put_at(b, EL_CANOPY, sx, -K.sign_out, bar, K.sign_t, arm, red, sz + (K.sign_w - arm) / 2);
	put_at(b, EL_CANOPY, sx, -K.sign_out, arm, K.sign_t, bar, red, sz + (K.sign_w - bar) / 2);
	put_at(b, EL_CANOPY, w / 2, -K.canopy_d / 2, K.canopy_w, K.canopy_d, K.canopy_t, b.add_paint(data.color("canopy")), b.ground_h - K.canopy_t);
}

// The window geometry compose_facades would give (the style's mid sizes), for the volumes kept out of it.
void civic_faces(const Data &data, Body &b) {
	const StyleDef &st = data.styles[b.style];
	for (Volume &v : b.volumes)
		for (int side = 0; side < v.sides() && side < MAX_FACES; side++) {
			FaceGeometry &g = v.faces[side];
			if (g.bays < 0) g.bays = columns_for(side_length(v, side), st.module);
			if (g.window_w <= 0) {
				g.window_w = (st.win_w0 + st.win_w1) * HALF;
				g.window_h = (st.win_h0 + st.win_h1) * HALF;
				g.sill = st.sill;
			}
		}
}

LotPlan civic_plan(const CivicShape &K, float W, float D) {
	LotPlan p;
	p.kind = KIND_SHOP;
	p.W = W;
	p.D = D;
	p.front = FRONT_PLAZA;
	p.front_depth = std::clamp(D * K.front_share, K.front_min, K.front_max);
	p.left = p.right = SidePlan{ SIDE_PATH, K.side_gap };
	p.back = BACK_NONE;
	p.x0 = K.side_gap;
	p.x1 = W - K.side_gap;
	p.y0 = p.front_depth;
	p.y1 = D - K.back_gap;
	return p;
}
} // namespace

void put(Body &b, Element k, float x0, float y0, float x1, float y1, float h, int paint, float z) {
	LotElement e;
	e.kind = k;
	e.x = (x0 + x1) / 2;
	e.y = (y0 + y1) / 2;
	e.w = std::fabs(x1 - x0);
	e.d = std::fabs(y1 - y0);
	e.h = h;
	e.z = z;
	e.paint = paint;
	b.elements.push_back(e);
}

bool is_civic(int fn) { return fn == FN_SCHOOL || fn == FN_CHURCH || fn == FN_HOSPITAL || fn == FN_CLINIC || is_service(fn); }
bool is_open_lot(int fn) { return fn == FN_SQUARE || fn == FN_PARK || fn == FN_SPORTS_COURT; }

void civic_body(const Data &data, Body &b, int fn, int floors, float W, float D, uint32_t seed) {
	const CivicShape &K = data.shapes.civic;
	b = Body{};
	b.kind = KIND_SHOP;
	b.use = USE_COMMERCIAL;
	b.ground = GROUND_LOBBY;
	Xoshiro128 r(seed);
	b.plan = civic_plan(data.shapes.civic, W, D);
	Envelope env;
	env.W = b.plan.x1 - b.plan.x0;
	env.D = b.plan.y1 - b.plan.y0;
	bool modern = fn == FN_HOSPITAL || fn == FN_CLINIC || fn == FN_SUPERMARKET || fn == FN_MALL || fn == FN_GAS_STATION || fn == FN_POLICE;
	b.style = modern ? STYLE_MODERNIST : STYLE_COLONIAL;
	Paint wall, trim;
	paints_of(data, b.style, r, wall, trim);
	const char *skin = fn == FN_SCHOOL ? "civic_wall" : fn == FN_CHURCH ? "church_wall" : fn == FN_HOSPITAL || fn == FN_CLINIC ? "hospital_wall"
			: fn == FN_CITY_HALL ? "civic_stone" : fn == FN_FIRE_STATION ? "fire_wall" : fn == FN_POLICE ? "police_wall" : nullptr;
	look(b, skin ? data.color(skin) : wall, data.color("office_roof"), trim);
	b.ground_h = b.storey_h = K.civic_storey;
	int door = -1;
	uint8_t door_kind = BAY_DOUBLE_DOOR;
	switch (fn) {
		case FN_SCHOOL: school(data, b, env.W, env.D, r); break;
		case FN_CHURCH: door = church(data, b, env.W, env.D); break;
		case FN_HOSPITAL: case FN_CLINIC: hospital(data, b, env.W, env.D, floors, fn == FN_CLINIC, r); door = 0; break;
		case FN_GAS_STATION: gas_station(data, b, r); door_kind = BAY_DOOR; break;
		case FN_SUPERMARKET: case FN_MALL: market(data, b, r, fn == FN_MALL); break;
		case FN_CITY_HALL: city_hall(data, b, r); break;
		default: station(data, b, r, fn == FN_FIRE_STATION); break;
	}
	civic_faces(data, b);
	finish_body(data, b, env, r, door_kind, door);
	float dx = b.plan.x0 - W / 2, dy = b.plan.y0;
	for (Volume &v : b.volumes) { v.x += dx; v.y += dy; }
	for (LotElement &e : b.elements) { e.x += dx; e.y += dy; }
	b.entrance_x += dx;
	lay_out_lot(data, b, r);
}

} // namespace city::b40
