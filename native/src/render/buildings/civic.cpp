// Civic bodies (Roadcraft jobs/buildingGen/civic.ts, drawn with the 4.0 facade grammar): a plaza in front, the
// building in an envelope set back from the street, then the function's own massing. School: an L of classrooms (2-3
// storeys x 3.6 m, cream, blue-grey hip roof) round a lawn court with a sports court. Church: a nave 9 m high under a
// steep clay gable with a 20 m bell tower, hip spire and gold cross in front. Hospital / clinic: a white ribbon-window
// block (5-9 / 2 storeys) on a glazed podium, blue band, red cross panel and an entrance canopy.
#include "render/buildings/civic.h"

#include "core/math.h"
#include "core/units.h"
#include "sim/buildings/building_store.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f, FRONT_SHARE = 0.18f, FRONT_MIN = 4.f, FRONT_MAX = 12.f, SIDE_GAP = 2.f, BACK_GAP = 2.f, CIVIC_STOREY = 3.6f;
// school
constexpr float WING_D = 11.f, WING_D_SHARE = 0.45f, SIDE_WING_W = 11.f, SIDE_WING_SHARE = 0.35f, SCHOOL_ROOF_PITCH = 24.f;
constexpr float COURT_W = 15.f, COURT_D = 28.f, COURT_LINE = 0.12f, COURT_EDGE = 1.5f, SURFACE_H = 0.06f;
constexpr int SCHOOL_STOREYS_LO = 2, SCHOOL_STOREYS_HI = 3;
// church
constexpr float NAVE_W = 14.f, NAVE_W_SHARE = 0.6f, NAVE_D = 30.f, NAVE_H = 9.f, NAVE_PITCH = 42.f, TOWER_W = 4.4f;
constexpr float TOWER_STOREY = 3.7f, TOWER_PITCH = 68.f, CROSS_H = 1.8f, CROSS_W = 1.f, CROSS_T = 0.18f, CROSS_ARM_Z = 0.65f;
constexpr int TOWER_STOREYS = 4;
// hospital
constexpr float WARD_SHARE = 0.8f, WARD_D = 22.f, WARD_D_SHARE = 0.55f, PODIUM_D_SHARE = 0.75f, SIGN_W = 3.2f, SIGN_T = 0.25f;
constexpr float SIGN_BAR = 0.75f, SIGN_OUT = 0.3f, BAND_H = 0.7f, BAND_OUT = 0.12f, CANOPY_W = 7.f, CANOPY_D = 4.f, CANOPY_T = 0.35f;
constexpr int HOSPITAL_LO = 5, HOSPITAL_HI = 9, CLINIC_STOREYS = 2;

bool is_service(int fn) {
	return fn == FN_GAS_STATION || fn == FN_SUPERMARKET || fn == FN_MALL || fn == FN_CITY_HALL || fn == FN_POLICE || fn == FN_FIRE_STATION;
}

void school(const Data &data, Body &b, float w, float d, Xoshiro128 &r) {
	int storeys = r.irange(SCHOOL_STOREYS_LO, SCHOOL_STOREYS_HI);
	float wd = std::min(WING_D, d * WING_D_SHARE), sw = std::min(SIDE_WING_W, w * SIDE_WING_SHARE);
	bool left = r.f() < HALF;
	int roof = b.add_paint(data.color("school_roof"));
	int a = add_block(b, { 0, 0, w, wd, 0, storeys, ROOF_HIP, BAY_WIDE, BAY_WIDE });
	int c = add_block(b, { left ? 0 : w - sw, wd, sw, d - wd, 0, storeys, ROOF_HIP, BAY_WIDE, BAY_WIDE });
	for (int i : { a, c }) {
		Volume &v = b.volumes[size_t(i)];
		v.pitch = SCHOOL_ROOF_PITCH;
		v.roof_paint = roof;
		v.lines = LINES_BASE;
		v.crown = CROWN_NONE;
		v.keep = true;
	}
	// the court: lawn round a painted sports court on the open side of the L
	float x0 = left ? sw : 0, x1 = left ? w : w - sw;
	put(b, EL_SURFACE, x0, wd, x1, d, SURFACE_H, b.add_paint(data.color("lawn")));
	float cw = std::min(COURT_W, x1 - x0 - 2 * COURT_EDGE), cd = std::min(COURT_D, d - wd - 2 * COURT_EDGE);
	if (cw <= 0 || cd <= 0) return;
	float cx = (x0 + x1) / 2, cy = (wd + d) / 2;
	put_at(b, EL_SURFACE, cx, cy, cw, cd, SURFACE_H * 2, b.add_paint(data.color("court")));
	int line = b.add_paint(data.color("court_line"));
	put_at(b, EL_SURFACE, cx, cy, cw, COURT_LINE, SURFACE_H * 3, line);
	for (float s : { -1.f, 1.f }) {
		put_at(b, EL_SURFACE, cx + s * (cw - COURT_LINE) / 2, cy, COURT_LINE, cd, SURFACE_H * 3, line);
		put_at(b, EL_SURFACE, cx, cy + s * (cd - COURT_LINE) / 2, cw, COURT_LINE, SURFACE_H * 3, line);
	}
}

void cross(Body &b, float x, float y, float z, int paint) {
	put_at(b, EL_CANOPY, x, y, CROSS_T, CROSS_T, CROSS_H, paint, z);
	put_at(b, EL_CANOPY, x, y, CROSS_W, CROSS_T, CROSS_T, paint, z + CROSS_H * CROSS_ARM_Z);
}

int church(const Data &data, Body &b, float w, float d) {
	b.ground_h = NAVE_H;
	b.storey_h = TOWER_STOREY;
	float nw = std::min(NAVE_W, w * NAVE_W_SHARE), nd = std::min(NAVE_D, d - TOWER_W / 2);
	float cx = w / 2;
	int nave = add_block(b, { cx - nw / 2, TOWER_W / 2, nw, nd, 0, 1, ROOF_GABLE, BAY_WINDOW, BAY_WINDOW });
	int tower = add_block(b, { cx - TOWER_W / 2, 0, TOWER_W, TOWER_W, 0, 1 + TOWER_STOREYS, ROOF_HIP, BAY_WINDOW, BAY_DOUBLE_DOOR });
	int roof = b.add_paint(data.color("church_roof"));
	Volume &n = b.volumes[size_t(nave)];
	n.ridge = 1;
	n.pitch = NAVE_PITCH;
	n.roof_paint = roof;
	Volume &t = b.volumes[size_t(tower)];
	t.pitch = TOWER_PITCH;
	t.roof_paint = roof;
	for (Volume *v : { &n, &t }) {
		v->keep = true;
		v->lines = LINES_EVERY;
		v->crown = CROWN_NONE;
	}
	float top = b.level_z(t.top()) + TOWER_W / 2 * std::tan(TOWER_PITCH * DEG_TO_RAD);
	cross(b, cx, TOWER_W / 2, top, b.add_paint(data.color("gold")));
	cross(b, cx, TOWER_W / 2 + nd, b.level_z(1) + nw / 2 * std::tan(NAVE_PITCH * DEG_TO_RAD), b.add_paint(data.color("gold")));
	return tower;
}

void hospital(const Data &data, Body &b, float w, float d, int floors, bool clinic, Xoshiro128 &r) {
	int storeys = clinic ? CLINIC_STOREYS : std::clamp(floors, HOSPITAL_LO, HOSPITAL_HI);
	if (!clinic && floors < HOSPITAL_LO) storeys = r.irange(HOSPITAL_LO, HOSPITAL_HI);
	float ww = w * WARD_SHARE, wd = std::min(WARD_D, d * WARD_D_SHARE), x = (w - ww) / 2;
	int podium = add_block(b, { 0, 0, w, d * PODIUM_D_SHARE, 0, 1, ROOF_FLAT, BAY_RIBBON, BAY_SHOPFRONT });
	int ward = add_block(b, { x, 0, ww, wd, 1, storeys - 1, ROOF_FLAT, BAY_RIBBON, BAY_NONE });
	for (int i : { podium, ward }) {
		Volume &v = b.volumes[size_t(i)];
		v.keep = true;
		v.lines = LINES_SLAB;
		v.crown = CROWN_SLAB;
	}
	float top = b.level_z(storeys);
	int band = b.add_paint(data.color("hospital_band")), red = b.add_paint(data.color("red_cross")), white = b.add_paint(data.color("sign_white"));
	put(b, EL_CANOPY, x - BAND_OUT, -BAND_OUT, x + ww + BAND_OUT, 0, BAND_H, band, top - BAND_H * 2);
	put(b, EL_CANOPY, -BAND_OUT, -BAND_OUT, w + BAND_OUT, 0, BAND_H, band, b.level_z(1) - BAND_H);
	float sx = x + ww - SIGN_W, sz = top - BAND_H * 3 - SIGN_W, arm = SIGN_W * SIGN_BAR, bar = SIGN_W * (1 - SIGN_BAR);
	put_at(b, EL_CANOPY, sx, -SIGN_OUT / 2, SIGN_W, SIGN_T, SIGN_W, white, sz);
	put_at(b, EL_CANOPY, sx, -SIGN_OUT, bar, SIGN_T, arm, red, sz + (SIGN_W - arm) / 2);
	put_at(b, EL_CANOPY, sx, -SIGN_OUT, arm, SIGN_T, bar, red, sz + (SIGN_W - bar) / 2);
	put_at(b, EL_CANOPY, w / 2, -CANOPY_D / 2, CANOPY_W, CANOPY_D, CANOPY_T, b.add_paint(data.color("canopy")), b.ground_h - CANOPY_T);
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

LotPlan civic_plan(float W, float D) {
	LotPlan p;
	p.kind = KIND_SHOP;
	p.W = W;
	p.D = D;
	p.front = FRONT_PLAZA;
	p.front_depth = std::clamp(D * FRONT_SHARE, FRONT_MIN, FRONT_MAX);
	p.left = p.right = SidePlan{ SIDE_PATH, SIDE_GAP };
	p.back = BACK_NONE;
	p.x0 = SIDE_GAP;
	p.x1 = W - SIDE_GAP;
	p.y0 = p.front_depth;
	p.y1 = D - BACK_GAP;
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
	b = Body{};
	b.kind = KIND_SHOP;
	b.use = USE_COMMERCIAL;
	b.ground = GROUND_LOBBY;
	Xoshiro128 r(seed);
	b.plan = civic_plan(W, D);
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
	b.ground_h = b.storey_h = CIVIC_STOREY;
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
	furnish_lot(data, b, r);
}

} // namespace city::b40
