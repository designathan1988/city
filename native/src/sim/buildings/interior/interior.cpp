// A floor of a building: its type (levels alike are furnished once), what each block of it holds by the building's purpose,
// a ceiling light over each group of pieces, and the homes or shops it is divided in.
#include "sim/buildings/interior/plan.h"

#include "sim/buildings/building_store.h"
#include "sim/buildings/b40/gen.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float FLIGHT_LEAST_M = .3f, FLIGHT_OVER_M = .05f, FLIGHT_LINE_M = 1.2f, MEET_M = 1e-3f, FLIGHT_RISE_M = .18f, RISER_SLACK = 1e-3f, FLIGHT_ROOM_M = .6f;
constexpr float FLIGHT_TREAD_M = .28f, FLIGHT_TREAD_MIN_M = .24f, FLIGHT_WIDTH_M = 1.f, FLIGHT_SIDE_M = .4f, FLIGHT_CLEAR_M = .2f;
constexpr float ROOM_REACH = 4.5f, UNIT_MODULES = 4.f, SPLIT_EPS = 1e-6f;
constexpr int OFFICE_ROOM_BAYS = 2, CLASS_ROOM_BAYS = 3, HOTEL_ROOM_BAYS = 1, WARD_ROOM_BAYS = 2, LIGHT_SHARE = 10, WOOD_SHARE = 3;
constexpr float WARD_FROM_GROUND = 10.f;
constexpr uint32_t PICK_SALT = 0x9e3779b1u, LOW_SHOP_MAX_FLOORS = 3;
constexpr int PICK_SHIFT = 16;
// A street shop's trade (the low and the taller shop), eight picks each.
constexpr Purpose LOW_SHOPS[] = { PU_SHOP, PU_SHOP, PU_SHOP, PU_RESTAURANT, PU_BAR, PU_SHOP, PU_RESTAURANT, PU_SUPERMARKET };
constexpr Purpose TALL_SHOPS[] = { PU_SHOP, PU_RESTAURANT, PU_BANK, PU_HOTEL, PU_GYM, PU_OFFICE, PU_SHOP, PU_SUPERMARKET };
constexpr int SHOP_PICKS = 8;

float module_of(const Data &data, const Body &b) { return data.styles[size_t(b.style) % data.styles.size()].module; }

float at_ref(const Plan &p, const Ref &r) {
	const float base[EDGE_COUNT] = { p.x0, p.x1, p.y0, p.y1, p.cx(), p.cy(), p.y_start() };
	return base[r.edge] + r.off;
}

void run_recipe(Plan &p, const std::vector<Step> &steps) {
	for (const Step &s : steps) {
		if ((s.level == LEVEL_GROUND && p.level != 0) || (s.level == LEVEL_UPPER && p.level == 0)) continue;
		const float a = at_ref(p, s.a), b = at_ref(p, s.b), c = at_ref(p, s.c), d = at_ref(p, s.d);
		switch (s.op) {
			case STEP_PUT: p.put(s.kind, a, b, s.angle); break;
			case STEP_GRID: p.grid(s.kind, a, b, c, d, s.wide ? std::max(s.gx, p.width() / 2) : s.gx, s.gy, s.angle); break;
			case STEP_BACK: p.along_back(s.kind, a, c, s.gx); break;
			case STEP_SIDE_LEFT: p.along_side(s.kind, true, a, c, s.gx); break;
			case STEP_SIDE_RIGHT: p.along_side(s.kind, false, a, c, s.gx); break;
			case STEP_WALL_ACROSS: p.wall_across(b, p.x0, p.x1, a); break;
			case STEP_LOBBY: lobby(p); break;
			default: break;
		}
	}
}

// The homes, shops or offices of one block's floor.
void spaces_of(const Body &b, const Volume &v, int vol, int level, float module, bool residential, std::vector<Space> &out) {
	if (residential && !(level == 0 && b.ground == GROUND_SHOP)) {
		const FlatPlan plan = flat_plan(v, 2);
		if (plan.flats.size() > 1 || plan.corridor) {
			for (const FlatCell &f : plan.flats) out.push_back({ vol, f.x, f.y, f.w, f.d });
			return;
		}
	}
	const bool along_x = v.w >= v.d;
	const float length = along_x ? v.w : v.d;
	const int split = residential ? std::max(1, int(std::ceil(length / (UNIT_MODULES * module) - SPLIT_EPS))) : 1;
	for (int n = 0; n < split; n++) {
		const float s0 = length * float(n) / float(split), size = length / float(split);
		out.push_back(along_x ? Space{ vol, v.x + s0, v.y, size, v.d } : Space{ vol, v.x, v.y + s0, v.w, size });
	}
}

void furnish(Plan &p, Purpose fn, const std::vector<Space> &spaces) {
	const auto &recipes = p.data.interior.recipes;
	const float from = p.y_start();
	run_recipe(p, recipes[fn]);
	switch (fn) {
		case PU_HOUSE: house_floor(p); break;
		case PU_FLATS:
			if (p.level == 0) lobby(p);
			else if (!flat_floor(p)) home_units(p, spaces, false);
			break;
		case PU_HOTEL:
			if (p.level == 0 || !rooms_floor(p, HOTEL_ROOM_BAYS, hotel_room)) home_units(p, spaces, true);
			break;
		case PU_OFFICE: case PU_CITY_HALL: case PU_POLICE: case PU_COURTHOUSE: case PU_POST_OFFICE:
			if (p.level > 0 && !rooms_floor(p, OFFICE_ROOM_BAYS, office_room)) office_desks(p);
			break;
		case PU_HOSPITAL:
			if (p.level == 0) wards(p, p.y0 + WARD_FROM_GROUND);
			else if (!rooms_floor(p, WARD_ROOM_BAYS, ward_room)) wards(p, p.y0);
			break;
		case PU_SCHOOL:
			if (!rooms_floor(p, CLASS_ROOM_BAYS, class_room)) classrooms(p, from);
			break;
		case PU_PRISON: cells(p, from); break;
		case PU_SHOP:
			if (p.level == 0 || !flat_floor(p)) shop_floor(p);
			break;
		case PU_RESTAURANT: case PU_BAR:
			if (p.level == 0 || !flat_floor(p)) eating_floor(p, fn == PU_BAR);
			break;
		case PU_MALL: shops_around(p, from); break;
		case PU_FIRE_STATION:
			if (p.level > 0) home_units(p, spaces, true);
			break;
		default: break;
	}
}

// A ceiling light over each group of furniture: a room's worth.
void room_lights(const Data &data, std::vector<Furniture> &items) {
	struct Room { float x, y; int n; };
	std::vector<Room> rooms;
	for (const Furniture &f : items) {
		auto near = std::find_if(rooms.begin(), rooms.end(), [&](const Room &r) { return std::hypot(r.x / float(r.n) - f.x, r.y / float(r.n) - f.y) < ROOM_REACH; });
		if (near != rooms.end()) { near->x += f.x; near->y += f.y; near->n++; }
		else rooms.push_back({ f.x, f.y, 1 });
	}
	const float *s = data.interior.size[FK_CEILING_LAMP];
	for (const Room &r : rooms) items.push_back({ FK_CEILING_LAMP, r.x / float(r.n), r.y / float(r.n), s[0], s[1], s[2], 0 });
}
} // namespace

Purpose purpose_of(const Body &b, int fn, uint32_t seed) {
	switch (fn) {
		case FN_SCHOOL: return PU_SCHOOL;
		case FN_CHURCH: return PU_CHURCH;
		case FN_HOSPITAL: case FN_CLINIC: return PU_HOSPITAL;
		case FN_GAS_STATION: return PU_SHOP;
		case FN_SUPERMARKET: return PU_SUPERMARKET;
		case FN_CITY_HALL: return PU_CITY_HALL;
		case FN_POLICE: return PU_POLICE;
		case FN_FIRE_STATION: return PU_FIRE_STATION;
		case FN_MALL: return PU_MALL;
		case FN_COUNCIL: return PU_COUNCIL;
		case FN_COURTHOUSE: return PU_COURTHOUSE;
		case FN_POST_OFFICE: return PU_POST_OFFICE;
		case FN_UNIVERSITY: return PU_SCHOOL;
		case FN_LIBRARY: return PU_LIBRARY;
		case FN_MUSEUM: return PU_MUSEUM;
		case FN_PRISON: return PU_PRISON;
		case FN_BUS_STATION: return PU_BUS_STATION;
		case FN_SHOP: case FN_PHARMACY: case FN_BAKERY: return PU_SHOP;
		case FN_BANK: return PU_BANK;
		case FN_RESTAURANT: case FN_SNACK_BAR: return PU_RESTAURANT;
		case FN_BAR: return PU_BAR;
		case FN_NIGHTCLUB: return PU_NIGHTCLUB;
		case FN_CINEMA: return PU_CINEMA;
		case FN_HOTEL: return PU_HOTEL;
		case FN_GYM: return PU_GYM;
		case FN_CLUB: return PU_CLUB;
		case FN_OFFICE: return PU_OFFICE;
		case FN_FACTORY: return PU_FACTORY;
		case FN_WAREHOUSE: return PU_WAREHOUSE;
		default: break;
	}
	switch (b.kind) {
		case KIND_HOUSE: return PU_HOUSE;
		case KIND_FLATS: case KIND_TOWER: return PU_FLATS;
		case KIND_OFFICE: return PU_OFFICE;
		case KIND_WORKS: return (seed & 1) != 0 ? PU_FACTORY : PU_WAREHOUSE;
		default: break;
	}
	int top = 0;
	for (const Volume &v : b.volumes) top = std::max(top, v.top());
	const uint32_t pick = ((seed * PICK_SALT) >> PICK_SHIFT) % SHOP_PICKS;
	return uint32_t(top) <= LOW_SHOP_MAX_FLOORS ? LOW_SHOPS[pick] : TALL_SHOPS[pick];
}

FloorFinish floor_finish(Purpose p, int level, uint32_t seed) {
	const bool homely = p == PU_HOUSE || p == PU_FLATS || p == PU_HOTEL;
	if (!(homely && (level > 0 || p == PU_HOUSE))) return FLOOR_STONE;
	return int((seed * PICK_SALT >> PICK_SHIFT) % LIGHT_SHARE) < WOOD_SHARE ? FLOOR_WOOD : FLOOR_PORCELAIN;
}

float lift_at(const Body &b, int level, float x, float y) {
	const Volume *best = nullptr;
	for (const Volume &v : b.volumes) {
		if (level < v.base || level >= v.top() || x < v.x || x > v.x + v.w || y < v.y || y > v.y + v.d) continue;
		if (best == nullptr || v.w * v.d < best->w * best->d) best = &v;
	}
	return best != nullptr ? best->lift : 0;
}

std::vector<LevelFlight> level_flights(const Body &b, int level) {
	std::vector<LevelFlight> out;
	std::vector<const Volume *> blocks;
	for (const Volume &v : b.volumes)
		if (v.outline.empty() && v.base <= level && v.top() > level) blocks.push_back(&v);
	for (size_t i = 0; i < blocks.size(); i++)
		for (size_t k = i + 1; k < blocks.size(); k++) {
			const float zp = b.block_z(*blocks[i], level), zq = b.block_z(*blocks[k], level), rise = std::fabs(zq - zp);
			if (rise < FLIGHT_LEAST_M || rise > b.level_h(level) + FLIGHT_OVER_M) continue;
			const Volume &low = zp < zq ? *blocks[i] : *blocks[k], &high = zp < zq ? *blocks[k] : *blocks[i];
			// The line they meet along: a side of the lower block against a side of the higher.
			const float ox0 = std::max(low.x, high.x), ox1 = std::min(low.x + low.w, high.x + high.w), oy0 = std::max(low.y, high.y), oy1 = std::min(low.y + low.d, high.y + high.d);
			float a = 0, c = 0, at = 0, inward = 0;
			bool along_x = false;
			if (ox1 - ox0 > FLIGHT_LINE_M && std::fabs(low.y + low.d - high.y) < MEET_M) { a = ox0; c = ox1; at = low.y + low.d; inward = -1; along_x = true; }
			else if (ox1 - ox0 > FLIGHT_LINE_M && std::fabs(high.y + high.d - low.y) < MEET_M) { a = ox0; c = ox1; at = low.y; inward = 1; along_x = true; }
			else if (oy1 - oy0 > FLIGHT_LINE_M && std::fabs(low.x + low.w - high.x) < MEET_M) { a = oy0; c = oy1; at = low.x + low.w; inward = -1; }
			else if (oy1 - oy0 > FLIGHT_LINE_M && std::fabs(high.x + high.w - low.x) < MEET_M) { a = oy0; c = oy1; at = low.x; inward = 1; }
			else continue;
			const int risers = int(std::ceil(rise / FLIGHT_RISE_M - RISER_SLACK));
			const float room = (along_x ? low.d : low.w) - FLIGHT_ROOM_M, tread = std::min(FLIGHT_TREAD_M, room / float(risers));
			if (tread < FLIGHT_TREAD_MIN_M) continue;
			const float run = tread * float(risers), mid = (a + c) / 2;
			LevelFlight f;
			f.top = along_x ? V2{ mid, at } : V2{ at, mid };
			f.foot = along_x ? V2{ mid, at + inward * run } : V2{ at + inward * run, mid };
			f.z0 = std::min(zp, zq);
			f.z1 = std::max(zp, zq);
			f.width = std::min(FLIGHT_WIDTH_M, c - a - FLIGHT_SIDE_M);
			f.risers = risers;
			out.push_back(f);
		}
	return out;
}

std::string Interior::type_of(int level) const {
	if (body_.furnishing.count(level) != 0) return "p" + std::to_string(level); // a floor the player arranged is like no other
	std::string key = level == 0 ? "g" : "u";
	for (size_t i = 0; i < body_.cores.size(); i++)
		if (body_.cores[i].to >= level) key += 'c' + std::to_string(i);
	for (size_t i = 0; i < body_.volumes.size(); i++) {
		const Volume &v = body_.volumes[i];
		if (level < v.base || level >= v.top()) continue;
		const int k = level - v.base;
		key += '|' + std::to_string(i) + ':' + std::to_string(v.fill[size_t(k)]) + (k == 0 ? "f" : "") + (k == v.storeys - 1 ? "t" : "");
		if (v.lift != 0) key += '^' + std::to_string(v.lift);
	}
	return key;
}

const FloorInterior &Interior::at(int level) {
	auto known = levels_.find(level);
	if (known != levels_.end()) return *known->second;
	const std::string type = type_of(level);
	std::unique_ptr<FloorInterior> &floor = types_[type];
	if (!floor) {
		floor = std::make_unique<FloorInterior>();
		const float module = module_of(data_, body_);
		const bool residential = purpose_ == PU_HOUSE || purpose_ == PU_FLATS;
		auto stored = body_.furnishing.find(level); // a floor the player furnished keeps the made partitions and spaces, its own pieces
		for (size_t i = 0; i < body_.volumes.size(); i++) {
			const Volume &v = body_.volumes[i];
			if (level < v.base || level >= v.top()) continue;
			std::vector<Space> spaces;
			spaces_of(body_, v, int(i), level, module, residential, spaces);
			Plan plan(data_, body_, v, level, module);
			plan.walls_only = stored != body_.furnishing.end();
			furnish(plan, purpose_, spaces);
			floor->furniture.insert(floor->furniture.end(), plan.items.begin(), plan.items.end());
			floor->partitions.insert(floor->partitions.end(), plan.walls.begin(), plan.walls.end());
			floor->spaces.insert(floor->spaces.end(), spaces.begin(), spaces.end());
		}
		// Nothing made for the floor stands on the flight between its levels.
		for (const LevelFlight &f : level_flights(body_, level)) {
			const float x0 = std::min(f.foot.x, f.top.x) - f.width / 2 - FLIGHT_CLEAR_M, x1 = std::max(f.foot.x, f.top.x) + f.width / 2 + FLIGHT_CLEAR_M;
			const float y0 = std::min(f.foot.y, f.top.y) - f.width / 2 - FLIGHT_CLEAR_M, y1 = std::max(f.foot.y, f.top.y) + f.width / 2 + FLIGHT_CLEAR_M;
			auto on_it = [&](const Furniture &p) { return p.x + p.w / 2 > x0 && p.x - p.w / 2 < x1 && p.y + p.d / 2 > y0 && p.y - p.d / 2 < y1; };
			floor->furniture.erase(std::remove_if(floor->furniture.begin(), floor->furniture.end(), on_it), floor->furniture.end());
		}
		if (stored == body_.furnishing.end()) room_lights(data_, floor->furniture);
		else {
			for (const PlacedPiece &p : stored->second) {
				const float *size = data_.interior.size[p.kind];
				floor->furniture.push_back({ FurnitureKind(p.kind), p.x, p.y, size[0], size[1], size[2], p.angle });
			}
		}
	}
	levels_[level] = floor.get();
	return *floor;
}

} // namespace city::b40
