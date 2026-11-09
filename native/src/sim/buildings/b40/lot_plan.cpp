// The plan of a grown lot: front, sides and back uses and the building envelope, by the lot's kind and size.
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/paint.h"

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f;

float gap(Xoshiro128 &r, float lo, float hi) { return (hi - lo) * float(r.f()) + lo; }
void sides(LotPlan &p, bool flip, SidePlan a, SidePlan b) {
	p.left = flip ? b : a;
	p.right = flip ? a : b;
}
// Rows of car bays a back yard holds after the front and the least building, each row with its share of the aisle.
int rows_within(const LotRules &l, float D, float front, float min_b) {
	float spare = D - front - min_b;
	if (spare >= l.row_depth(2)) return 2;
	return spare >= l.row_depth(1) ? 1 : 0;
}

void plan_house(const LotPlanShape &K, LotPlan &p, Xoshiro128 &r, bool flip) {
	bool narrow = p.W < K.narrow_w;
	if (p.D >= K.deep_house) {
		if (narrow) { p.front = FRONT_CARPAD; p.front_depth = K.carpad; }
		else { p.front = FRONT_GARDEN; p.front_depth = half_down(gap(r, K.garden_lo, K.garden_hi)); }
	} else {
		p.front = FRONT_GARDEN;
		p.front_depth = p.D >= K.mid_house ? K.small_garden : 0;
	}
	if (p.front_depth == 0) p.front = FRONT_STREET;
	SidePlan alley{ SIDE_PATH, K.alley };
	if (narrow) sides(p, flip, { SIDE_PATH, K.footpath }, alley);
	else {
		SidePlan other = alley;
		if (p.W >= K.side_garden_w) other = { SIDE_GARDEN, half_down(gap(r, K.side_garden_lo, K.side_garden_hi)) };
		else if (p.W >= K.side_path_w) other = { SIDE_PATH, K.footpath };
		sides(p, flip, { SIDE_DRIVE, K.house_drive }, other);
	}
	float house_d = std::min(std::max(K.house_min, p.D - p.front_depth - gap(r, K.yard_lo, K.yard_hi)), K.house_max);
	float yard = p.D - p.front_depth - house_d;
	if (yard >= K.yard_min) { p.back = BACK_YARD; p.back_depth = half_down(yard); }
}

void plan_flats(const LotPlanShape &K, const LotRules &l, LotPlan &p, Xoshiro128 &r, bool flip) {
	if (p.kind == KIND_TOWER) {
		bool plaza = p.D >= K.tower_plaza_d;
		p.front = plaza ? FRONT_PLAZA : FRONT_STREET;
		p.front_depth = plaza ? K.tower_plaza : 0;
	} else if (p.W >= K.apartment_w && p.D >= K.flats_d && r.f() < K.flats_garden) {
		p.front = FRONT_GARDEN;
		p.front_depth = half_down(gap(r, K.flats_garden_lo, K.flats_garden_hi));
	}
	int stalls = p.W >= K.parking_w ? rows_within(l, p.D, p.front_depth, K.min_block) : 0;
	if (stalls > 0) {
		SidePlan b = r.f() < K.attached_share ? SidePlan{} : SidePlan{ SIDE_PATH, K.wide_path };
		sides(p, flip, { SIDE_DRIVE_PATH, K.drive + K.footpath, K.footpath }, b);
		p.back = BACK_PARKING; p.back_depth = l.row_depth(stalls); p.rows = stalls;
		return;
	}
	sides(p, flip, p.W >= K.apartment_w ? SidePlan{ SIDE_PATH, K.wide_path } : SidePlan{}, SidePlan{});
	float spare = p.D - p.front_depth - K.min_block;
	if (spare >= K.court_spare) { p.back = BACK_COURTYARD; p.back_depth = half_down(std::min(spare, K.court_max)); }
}

void plan_shop(const LotPlanShape &K, const LotRules &l, LotPlan &p, Xoshiro128 &r, bool flip) {
	if (p.D >= K.shop_court_d && r.f() < K.shop_court) { p.front = FRONT_FORECOURT; p.front_depth = half_down(gap(r, K.shop_court_lo, K.shop_court_hi)); }
	int stalls = p.W >= K.shop_park_w && r.f() < K.shop_park ? std::min(1, rows_within(l, p.D, p.front_depth, K.min_block)) : 0;
	if (stalls > 0) {
		sides(p, flip, { SIDE_DRIVE, K.drive }, SidePlan{});
		p.back = BACK_PARKING; p.back_depth = l.row_depth(1); p.rows = stalls;
	} else if (p.D - p.front_depth >= K.shop_service) {
		p.back = BACK_SERVICE; p.back_depth = half_down(gap(r, K.service_lo, K.service_hi));
	}
}

void plan_office(const LotPlanShape &K, const LotRules &l, LotPlan &p, Xoshiro128 &r, bool flip) {
	if (p.D >= K.office_plaza_d) { p.front = FRONT_PLAZA; p.front_depth = half_down(gap(r, K.plaza_lo, K.plaza_hi)); }
	else { p.front = FRONT_FORECOURT; p.front_depth = p.D >= K.workplace_min ? K.office_court : 0; }
	if (p.front_depth == 0) p.front = FRONT_STREET;
	int stalls = p.W >= K.office_park_w ? rows_within(l, p.D, p.front_depth, K.workplace_min) : 0;
	if (stalls > 0) {
		sides(p, flip, { SIDE_DRIVE_PATH, K.drive + K.footpath, K.footpath }, SidePlan{});
		p.back = BACK_PARKING; p.back_depth = l.row_depth(stalls); p.rows = stalls;
	} else if (p.D - p.front_depth >= K.office_service) {
		p.back = BACK_SERVICE; p.back_depth = K.office_service_d;
	}
}

void plan_industry(const LotPlanShape &K, const LotRules &l, LotPlan &p, bool flip) {
	if (p.D >= K.apron_d) { p.front = FRONT_APRON; p.front_depth = p.D >= K.apron_deep ? l.row_depth(1) : K.apron; }
	if (p.W < K.works_w || p.D < K.works_d) return;
	sides(p, flip, { SIDE_DRIVE, K.truck_lane }, p.W >= K.works_path_w ? SidePlan{ SIDE_PATH, K.wide_path } : SidePlan{});
	float spare = p.D - p.front_depth;
	p.back = BACK_LOADING;
	p.back_depth = half_down(spare >= K.loading_full + K.loading_extra ? K.loading_full : std::min(K.loading_max, std::max(K.loading_min, spare * K.loading_share)));
}

bool drives(const SidePlan &s) { return s.use == SIDE_DRIVE || s.use == SIDE_DRIVE_PATH; }

void settle(const LotPlanShape &K, LotPlan &p) {
	if (p.back == BACK_PARKING && !drives(p.left) && !drives(p.right)) { p.back = BACK_SERVICE; p.back_depth = std::min(K.service_max, p.back_depth); }
	if (p.W - p.left.width - p.right.width >= K.narrow_lot) return;
	if (p.kind == KIND_HOUSE && p.W >= K.house_paths_w) {
		p.left = p.right = SidePlan{ SIDE_PATH, K.house_path };
		return;
	}
	p.left = p.right = SidePlan{};
	if (p.back == BACK_PARKING) { p.back = BACK_SERVICE; p.back_depth = std::min(K.service_max, p.back_depth); }
}

// A corner lot's side on the second street is a garden strip, not a party wall (lotPlan.ts planLot), where the building keeps its width.
void open_corner(const LotPlanShape &K, LotPlan &p) {
	if (p.corner == CORNER_NONE) return;
	SidePlan &side = p.corner == CORNER_LEFT ? p.left : p.right;
	const SidePlan &other = p.corner == CORNER_LEFT ? p.right : p.left;
	if (side.use == SIDE_ATTACHED && p.W - other.width - K.corner_garden >= K.narrow_lot) side = SidePlan{ SIDE_GARDEN, K.corner_garden };
}
} // namespace

LotPlan plan_lot(const Data &data, Kind kind, float W, float D, Xoshiro128 &r, LotCorner corner) {
	const LotRules &l = data.lots;
	const LotPlanShape &K = data.shapes.plan;
	LotPlan p;
	p.kind = kind; p.W = W; p.D = D; p.corner = corner;
	bool flip = r.f() < HALF;
	switch (kind) {
		case KIND_HOUSE: plan_house(K, p, r, flip); break;
		case KIND_FLATS:
		case KIND_TOWER: plan_flats(K, l, p, r, flip); break;
		case KIND_SHOP: plan_shop(K, l, p, r, flip); break;
		case KIND_OFFICE: plan_office(K, l, p, r, flip); break;
		default: plan_industry(K, l, p, flip); break;
	}
	settle(K, p);
	open_corner(K, p);
	p.x0 = p.left.width; p.y0 = p.front_depth; p.x1 = W - p.right.width; p.y1 = D - p.back_depth;
	return p;
}

} // namespace city::b40
