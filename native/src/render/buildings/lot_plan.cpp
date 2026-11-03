// The plan of a grown lot (4.0 world/growth/lotPlan.ts, R5 jobs/b40/lot/plan.ts): front, sides and back uses and the
// building envelope. Same rules and draw order; no corner gardens (R5 passes no corner).
#include "render/buildings/b40_gen.h"
#include "render/buildings/b40_paint.h"

namespace city::b40 {

namespace {
constexpr float DRIVE = 3.5f, HOUSE_DRIVE = 3.f, FOOTPATH = 1.2f, ALLEY = 1.1f, WIDE_PATH = 1.5f, TRUCK_LANE = 6.f;
constexpr float STALL = 5.f, AISLE = 6.f;
constexpr float NARROW_W = 10.5f, DEEP_HOUSE = 22.f, CARPAD = 5.5f, MID_HOUSE = 14.f, SMALL_GARDEN = 3.f;
constexpr float GARDEN_LO = 3.5f, GARDEN_HI = 5.5f, SIDE_GARDEN_W = 16.f, SIDE_GARDEN_LO = 3.f, SIDE_GARDEN_HI = 4.f;
constexpr float SIDE_PATH_W = 13.f, HOUSE_MIN = 8.f, HOUSE_MAX = 15.f, YARD_LO = 8.f, YARD_HI = 12.f, YARD_MIN = 4.f;
constexpr float APARTMENT_W = 14.f, FLATS_D = 20.f, FLATS_GARDEN = 0.75f, FLATS_GARDEN_LO = 3.f, FLATS_GARDEN_HI = 5.f;
constexpr float PARKING_W = 18.f, MIN_BLOCK = 12.f, ATTACHED_SHARE = 0.55f, COURT_SPARE = 5.f, COURT_MAX = 12.f;
constexpr float TOWER_PLAZA_D = 24.f, TOWER_PLAZA = 5.f;
constexpr float SHOP_COURT_D = 16.f, SHOP_COURT = 0.45f, SHOP_COURT_LO = 3.f, SHOP_COURT_HI = 4.5f, SHOP_PARK_W = 15.f;
constexpr float SHOP_PARK = 0.6f, SHOP_SERVICE = 14.f, SERVICE_LO = 4.5f, SERVICE_HI = 6.f;
constexpr float OFFICE_PLAZA_D = 22.f, PLAZA_LO = 5.f, PLAZA_HI = 7.f, OFFICE_COURT = 3.f, OFFICE_PARK_W = 16.f;
constexpr float WORKPLACE_MIN = 14.f, OFFICE_SERVICE = 16.f, OFFICE_SERVICE_D = 5.f;
constexpr float NARROW_LOT = 6.f, HOUSE_PATHS_W = 8.5f, HOUSE_PATH = 1.f, SERVICE_MAX = 6.f, HALF = 0.5f;
constexpr float APRON_D = 22.f, APRON_DEEP = 40.f, APRON = 6.f, WORKS_W = 20.f, WORKS_D = 30.f, WORKS_PATH_W = 26.f;
constexpr float LOADING_FULL = 22.f, LOADING_EXTRA = 14.f, LOADING_MAX = 16.f, LOADING_MIN = 12.f, LOADING_SHARE = 0.35f;

float gap(Xoshiro128 &r, float lo, float hi) { return (hi - lo) * float(r.f()) + lo; }
void sides(LotPlan &p, bool flip, SidePlan a, SidePlan b) {
	p.left = flip ? b : a;
	p.right = flip ? a : b;
}
int rows_within(float D, float front, float min_b) {
	float spare = D - front - min_b;
	if (spare >= STALL * 2 + AISLE) return 2;
	return spare >= STALL + AISLE ? 1 : 0;
}
float parking_depth(int rows) { return rows == 2 ? STALL * 2 + AISLE : STALL + AISLE; }

void plan_house(LotPlan &p, Xoshiro128 &r, bool flip) {
	bool narrow = p.W < NARROW_W;
	if (p.D >= DEEP_HOUSE) {
		if (narrow) { p.front = FRONT_CARPAD; p.front_depth = CARPAD; }
		else { p.front = FRONT_GARDEN; p.front_depth = half_down(gap(r, GARDEN_LO, GARDEN_HI)); }
	} else {
		p.front = FRONT_GARDEN;
		p.front_depth = p.D >= MID_HOUSE ? SMALL_GARDEN : 0;
	}
	if (p.front_depth == 0) p.front = FRONT_STREET;
	SidePlan alley{ SIDE_PATH, ALLEY };
	if (narrow) sides(p, flip, { SIDE_PATH, FOOTPATH }, alley);
	else {
		SidePlan other = alley;
		if (p.W >= SIDE_GARDEN_W) other = { SIDE_GARDEN, half_down(gap(r, SIDE_GARDEN_LO, SIDE_GARDEN_HI)) };
		else if (p.W >= SIDE_PATH_W) other = { SIDE_PATH, FOOTPATH };
		sides(p, flip, { SIDE_DRIVE, HOUSE_DRIVE }, other);
	}
	float house_d = std::min(std::max(HOUSE_MIN, p.D - p.front_depth - gap(r, YARD_LO, YARD_HI)), HOUSE_MAX);
	float yard = p.D - p.front_depth - house_d;
	if (yard >= YARD_MIN) { p.back = BACK_YARD; p.back_depth = half_down(yard); }
}

void plan_flats(LotPlan &p, Xoshiro128 &r, bool flip) {
	if (p.kind == KIND_TOWER) {
		bool plaza = p.D >= TOWER_PLAZA_D;
		p.front = plaza ? FRONT_PLAZA : FRONT_STREET;
		p.front_depth = plaza ? TOWER_PLAZA : 0;
	} else if (p.W >= APARTMENT_W && p.D >= FLATS_D && r.f() < FLATS_GARDEN) {
		p.front = FRONT_GARDEN;
		p.front_depth = half_down(gap(r, FLATS_GARDEN_LO, FLATS_GARDEN_HI));
	}
	int stalls = p.W >= PARKING_W ? rows_within(p.D, p.front_depth, MIN_BLOCK) : 0;
	if (stalls > 0) {
		SidePlan b = r.f() < ATTACHED_SHARE ? SidePlan{} : SidePlan{ SIDE_PATH, WIDE_PATH };
		sides(p, flip, { SIDE_DRIVE_PATH, DRIVE + FOOTPATH }, b);
		p.back = BACK_PARKING; p.back_depth = parking_depth(stalls); p.rows = stalls;
		return;
	}
	sides(p, flip, p.W >= APARTMENT_W ? SidePlan{ SIDE_PATH, WIDE_PATH } : SidePlan{}, SidePlan{});
	float spare = p.D - p.front_depth - MIN_BLOCK;
	if (spare >= COURT_SPARE) { p.back = BACK_COURTYARD; p.back_depth = half_down(std::min(spare, COURT_MAX)); }
}

void plan_shop(LotPlan &p, Xoshiro128 &r, bool flip) {
	if (p.D >= SHOP_COURT_D && r.f() < SHOP_COURT) { p.front = FRONT_FORECOURT; p.front_depth = half_down(gap(r, SHOP_COURT_LO, SHOP_COURT_HI)); }
	int stalls = p.W >= SHOP_PARK_W && r.f() < SHOP_PARK ? std::min(1, rows_within(p.D, p.front_depth, MIN_BLOCK)) : 0;
	if (stalls > 0) {
		sides(p, flip, { SIDE_DRIVE, DRIVE }, SidePlan{});
		p.back = BACK_PARKING; p.back_depth = STALL + AISLE; p.rows = stalls;
	} else if (p.D - p.front_depth >= SHOP_SERVICE) {
		p.back = BACK_SERVICE; p.back_depth = half_down(gap(r, SERVICE_LO, SERVICE_HI));
	}
}

void plan_office(LotPlan &p, Xoshiro128 &r, bool flip) {
	if (p.D >= OFFICE_PLAZA_D) { p.front = FRONT_PLAZA; p.front_depth = half_down(gap(r, PLAZA_LO, PLAZA_HI)); }
	else { p.front = FRONT_FORECOURT; p.front_depth = p.D >= WORKPLACE_MIN ? OFFICE_COURT : 0; }
	if (p.front_depth == 0) p.front = FRONT_STREET;
	int stalls = p.W >= OFFICE_PARK_W ? rows_within(p.D, p.front_depth, WORKPLACE_MIN) : 0;
	if (stalls > 0) {
		sides(p, flip, { SIDE_DRIVE_PATH, DRIVE + FOOTPATH }, SidePlan{});
		p.back = BACK_PARKING; p.back_depth = parking_depth(stalls); p.rows = stalls;
	} else if (p.D - p.front_depth >= OFFICE_SERVICE) {
		p.back = BACK_SERVICE; p.back_depth = OFFICE_SERVICE_D;
	}
}

void plan_industry(LotPlan &p, bool flip) {
	if (p.D >= APRON_D) { p.front = FRONT_APRON; p.front_depth = p.D >= APRON_DEEP ? STALL + AISLE : APRON; }
	if (p.W < WORKS_W || p.D < WORKS_D) return;
	sides(p, flip, { SIDE_DRIVE, TRUCK_LANE }, p.W >= WORKS_PATH_W ? SidePlan{ SIDE_PATH, WIDE_PATH } : SidePlan{});
	float spare = p.D - p.front_depth;
	p.back = BACK_LOADING;
	p.back_depth = half_down(spare >= LOADING_FULL + LOADING_EXTRA ? LOADING_FULL : std::min(LOADING_MAX, std::max(LOADING_MIN, spare * LOADING_SHARE)));
}

bool drives(const SidePlan &s) { return s.use == SIDE_DRIVE || s.use == SIDE_DRIVE_PATH; }

void settle(LotPlan &p) {
	if (p.back == BACK_PARKING && !drives(p.left) && !drives(p.right)) { p.back = BACK_SERVICE; p.back_depth = std::min(SERVICE_MAX, p.back_depth); }
	if (p.W - p.left.width - p.right.width >= NARROW_LOT) return;
	if (p.kind == KIND_HOUSE && p.W >= HOUSE_PATHS_W) {
		p.left = p.right = SidePlan{ SIDE_PATH, HOUSE_PATH };
		return;
	}
	p.left = p.right = SidePlan{};
	if (p.back == BACK_PARKING) { p.back = BACK_SERVICE; p.back_depth = std::min(SERVICE_MAX, p.back_depth); }
}
} // namespace

LotPlan plan_lot(Kind kind, float W, float D, Xoshiro128 &r) {
	LotPlan p;
	p.kind = kind; p.W = W; p.D = D;
	bool flip = r.f() < HALF;
	switch (kind) {
		case KIND_HOUSE: plan_house(p, r, flip); break;
		case KIND_FLATS:
		case KIND_TOWER: plan_flats(p, r, flip); break;
		case KIND_SHOP: plan_shop(p, r, flip); break;
		case KIND_OFFICE: plan_office(p, r, flip); break;
		default: plan_industry(p, flip); break;
	}
	settle(p);
	p.x0 = p.left.width; p.y0 = p.front_depth; p.x1 = W - p.right.width; p.y1 = D - p.back_depth;
	return p;
}

} // namespace city::b40
