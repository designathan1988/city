// Low-entry city bus (agents.json "bus", the rules and their sources in its _doc): a hollow body whose side walls are cut
// over the wheels and open at the doors on the kerb side (-X), the low floor from the front to the last door and a deck
// one or more steps up behind it, over the rear wheels and the engine. The glazed band runs between pillars no further
// apart than the widest window, the windscreen and the rear window on the same line; the destination sign over the
// windscreen, the air conditioning on the roof. Inside (agent_cabin): the driver's toes on the pedals at the front wall,
// the partition behind them; double seats either side of the aisle in rows as close as the free space ahead of each
// allows, none in a doorway nor in the wheelchair's place opposite the last door, those over a wheel on a platform over
// its housing; the back row across the deck; stanchions alternating along the aisle and the ceiling rails over it.
#include "render/agents/agent_meshes.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {

constexpr float DOOR_SPLIT_W = 0.04f, PARTITION_THICK_M = 0.04f;
constexpr float WELL_GAP_M = 0.02f; // the wheel wells clear of the twin tyres' inner faces

struct Bus {
	float L = 0, W = 0, H = 0, hw = 0, hl = 0, clear = 0, floor = 0, deck = 0, deck_z = 0, sill = 0, cant = 0, rear_sill = 0, wall = 0;
	float arch_r = 0, arch_top = 0, well_x = 0, pitch = 0, rise = 0, tread = 0;
	int steps = 0;
	Running g;
	std::vector<Vector2> doors; // kerb side, z spans
};

Bus read_bus(const Dictionary &d) {
	Bus b;
	b.L = d["length"]; b.W = d["width"]; b.H = d["height"]; b.clear = d["clearance"]; b.floor = d["floor"]; b.wall = d["wall_m"];
	b.hw = b.W * 0.5f; b.hl = b.L * 0.5f;
	b.g = heavy_running(d, b.L);
	b.arch_r = b.g.r + HEAVY_ARCH_PAD_M; b.arch_top = 2 * b.g.r + HEAVY_ARCH_PAD_M;
	b.well_x = (b.g.rear_track - b.g.dual - b.g.w) * 0.5f - WELL_GAP_M;
	const Vector2 step = agent_v2(d["step_m"]); // rise, tread
	b.rise = step.x; b.tread = step.y; b.steps = d["deck_steps"];
	b.deck = b.floor + b.steps * b.rise;
	b.sill = b.floor + float(d["sill_m"]); b.rear_sill = b.deck + float(d["sill_m"]);
	b.cant = b.deck + float(d["liner_m"]);
	b.pitch = d["window_max_m"];
	// the first door behind the windscreen's corner pillar, the second midway between the first two axles
	const float door = d["door_m"], z1 = b.hl - HEAVY_PILLAR_M, mid = (b.g.axles[0] + b.g.axles[1]) * 0.5f;
	b.doors = { Vector2(z1 - door, z1), Vector2(mid - door * 0.5f, mid + door * 0.5f) };
	// the deck from the last door's back edge, its steps' treads behind it on the low floor
	b.deck_z = b.doors.back().x - b.steps * b.tread;
	return b;
}

float floor_at(const Bus &b, float z) { return z < b.deck_z ? b.deck : b.floor; }

// Where the seats go: each seat's footprint behind and ahead of its hip point (its backrest; the cushion and the free
// space ahead), the aisle's edge, the driver's partition, the platforms over the wheel housings (per side, z spans).
struct Plan {
	float behind = 0, ahead = 0, aisle = 0, inner = 0, partition = 0, place = 0;
	Vector2 reserved; // the wheelchair's place on the driver's side opposite the last door (z span)
	Seat driver;
	std::vector<Vector2> platform[2]; // [0] the kerb side (-X), [1] the driver's
	std::vector<Seat> seats;
};

int sided(float x) { return x < 0 ? 0 : 1; }

bool in_doorway(const Bus &b, const Plan &p, float z) {
	for (const Vector2 &d : b.doors)
		if (z + p.ahead > d.x + CMP_EPSILON && z - p.behind < d.y - CMP_EPSILON) return true;
	return false;
}

// Rows of a pair of seats on one side, hip points from z0 back by the pitch to z1: each wholly on the deck or on the low
// floor, none on the kerb side in a doorway.
void side_rows(const Bus &b, Plan &p, int side, float z0, float z1, const CabinLook &l) {
	for (float z = z0; z >= z1 - CMP_EPSILON; z -= p.behind + p.ahead) {
		const bool deck = z + p.ahead <= b.deck_z + CMP_EPSILON, low = z - p.behind >= b.deck_z - CMP_EPSILON;
		const bool reserved = side > 0 && z + p.ahead > p.reserved.x && z - p.behind < p.reserved.y;
		if ((side < 0 && in_doorway(b, p, z)) || reserved || !(deck || low)) continue;
		for (int k = 0; k < 2; k++) {
			Seat s;
			s.x = side * (p.inner - p.place * (k + 0.5f));
			s.z = z;
			s.floor = deck ? b.deck : b.floor;
			s.style = &l.chair;
			p.seats.push_back(s);
		}
	}
}

// A seat whose footprint reaches over a wheel stands on a platform over its housing (ABNT NBR 15570 27.3.5), the
// platform as long as the housing and the seats on it.
void platforms(const Bus &b, Plan &p) {
	for (float ax : b.g.axles)
		for (int side : { -1, 1 }) {
			Vector2 span(ax - b.arch_r, ax + b.arch_r);
			for (const Seat &s : p.seats)
				if (sided(s.x) == sided(float(side)) && s.z + p.ahead > ax - b.arch_r && s.z - p.behind < ax + b.arch_r)
					span = Vector2(std::min(span.x, s.z - p.behind), std::max(span.y, s.z + p.ahead));
			p.platform[sided(float(side))].push_back(span);
			for (Seat &s : p.seats)
				if (sided(s.x) == sided(float(side)) && s.z - p.behind >= span.x - CMP_EPSILON && s.z + p.ahead <= span.y + CMP_EPSILON)
					s.floor = b.arch_top;
		}
}

Plan plan_of(const Bus &b, const Dictionary &in, const Cabin &c, const CabinLook &l) {
	Plan p;
	p.place = l.chair.width_m;
	p.behind = l.person.torso.z * 0.5f + l.chair.back_d_m;
	p.ahead = l.chair.depth_m * 0.5f + float(in["knee_m"]);
	p.inner = c.half_w - l.lining_inset_m;
	p.aisle = p.inner - 2 * p.place;
	p.driver.x = p.inner - p.place;
	p.driver.z = pedal_hip_z(c, l);
	p.driver.floor = b.floor;
	p.driver.style = &l.cab;
	p.driver.driver = true;
	p.partition = p.driver.z - l.person.torso.z * 0.5f - l.cab.back_d_m;
	const float wheelchair = in["wheelchair_m"], door = (b.doors.back().x + b.doors.back().y) * 0.5f;
	p.reserved = Vector2(door - wheelchair * 0.5f, door + wheelchair * 0.5f);
	const float back = c.z_back + l.lining_inset_m, last = back + p.behind; // the back row's hip
	side_rows(b, p, 1, p.partition - p.ahead, b.deck_z + p.behind, l);
	side_rows(b, p, -1, b.doors.front().x - p.ahead, b.deck_z + p.behind, l);
	for (int side : { 1, -1 }) // the deck's rows: from the back row forward
		for (float z = last + p.behind + p.ahead; z + p.ahead <= b.deck_z + CMP_EPSILON; z += p.behind + p.ahead) side_rows(b, p, side, z, z, l);
	platforms(b, p);
	const float single = in["single_m"]; // the back row: double, single, double
	for (float x : { -single * 0.5f - p.place * 1.5f, -single * 0.5f - p.place * 0.5f, 0.f, single * 0.5f + p.place * 0.5f, single * 0.5f + p.place * 1.5f }) {
		Seat s;
		s.x = x;
		s.z = last;
		s.floor = b.deck;
		s.style = &l.chair;
		p.seats.push_back(s);
	}
	return p;
}

// The deck over the rear and its steps up from the low floor, across the aisle; beside the aisle it gives way to the
// wheel housings over the axles.
void deck(MeshBuilder &mb, const Bus &b, const Plan &p, const Cabin &c) {
	const float w = c.half_w;
	agent_box(mb, Vector3(-p.aisle, b.floor, c.z_back), Vector3(p.aisle, b.deck, b.deck_z), c.paint.floor, AgentTag::FIXED);
	for (int side : { -1, 1 })
		agent_arched_box(mb, std::min(side * p.aisle, side * w), std::max(side * p.aisle, side * w), b.floor, b.deck, c.z_back, b.deck_z, b.g.axles, b.arch_r,
				b.arch_top, c.paint.floor, AgentTag::FIXED);
	for (int k = 1; k < b.steps; k++)
		agent_box(mb, Vector3(-p.aisle, b.floor, b.deck_z), Vector3(p.aisle, b.floor + k * b.rise, b.deck_z + (b.steps - k) * b.tread), c.paint.floor,
				AgentTag::FIXED);
}

// Each wheel housing: a block from its floor inboard of the wheel up to the arch's top (the platform), a slab over the
// wheel out to the wall, the well round the wheel dark. Seen through the arches too: at both levels (the far one without
// the block: no cabin to stand in).
void housings(MeshBuilder &mb, const Bus &b, const std::vector<Vector2> plat[2], float aisle, const Color &c, const AgentTrim &trim) {
	const float eps = HEAVY_PROUD_M * 0.5f, cap = b.arch_top - HEAVY_PROUD_M;
	for (int side : { -1, 1 })
		for (const Vector2 &s : plat[sided(float(side))]) {
			const float in = side * aisle, well = side * b.well_x, out = side * (b.hw - b.wall), base = floor_at(b, (s.x + s.y) * 0.5f);
			if (aisle < b.well_x) agent_box(mb, Vector3(in, base, s.x), Vector3(well, b.arch_top, s.y), c, AgentTag::FIXED);
			agent_box(mb, Vector3(well, cap, s.x), Vector3(out, b.arch_top, s.y), c, AgentTag::FIXED);
			const float xw = well + side * eps, yc = cap - eps;
			mb.quad(Vector3(xw, b.clear, s.x), Vector3(xw, b.clear, s.y), Vector3(xw, yc, s.y), Vector3(xw, yc, s.x), trim.under, Vector3(side, 0, 0), nullptr,
					agent_tag(AgentTag::FIXED));
			mb.quad(Vector3(well, yc, s.x), Vector3(out, yc, s.x), Vector3(out, yc, s.y), Vector3(well, yc, s.y), trim.under, Vector3(0, -1, 0), nullptr,
					agent_tag(AgentTag::FIXED));
		}
}

// Stanchions along the aisle's edges alternating sides at most pole_m[1] apart and one in each doorway, from the floor
// to the ceiling rails over the aisle's edges (rail_m over each floor), the partition behind the driver with glass above.
void fittings(MeshBuilder &mb, const Bus &b, const Plan &p, const Dictionary &in, const Cabin &c, const CabinLook &l, const AgentTrim &trim) {
	const Color pole = agent_hex(in["pole"]);
	const Vector2 pm = agent_v2(in["pole_m"]), wall = agent_v2(in["partition_m"]); // diameter, spacing; height, gap over the floor
	const float t = pm.x * 0.5f, rail = in["rail_m"], liner = c.roof - l.liner_drop_m, front = p.partition - PARTITION_THICK_M;
	auto rail_at = [&](float z) { return std::min(floor_at(b, z) + rail, liner - pm.x); };
	auto stanchion = [&](float x, float z) { // in the aisle, clear of the seats
		const float xc = x - (x > 0 ? t : -t);
		agent_box(mb, Vector3(xc - t, floor_at(b, z), z - t), Vector3(xc + t, rail_at(z), z + t), pole, AgentTag::FIXED);
	};
	int side = 1;
	for (float z = front - t; z > c.z_back + p.behind; z -= pm.y * 0.5f, side = -side)
		if (side > 0 || !in_doorway(b, p, z)) stanchion(side * p.aisle, z);
	for (const Vector2 &d : b.doors) stanchion(-p.aisle, (d.x + d.y) * 0.5f);
	for (int s : { -1, 1 })
		for (const Vector2 &span : { Vector2(b.deck_z, front), Vector2(c.z_back + p.behind, b.deck_z) }) {
			const float y = rail_at((span.x + span.y) * 0.5f);
			const float x = s * (p.aisle - t);
			agent_box(mb, Vector3(x - t, y - pm.x, span.x), Vector3(x + t, y, span.y), pole, AgentTag::FIXED);
		}
	const float x0 = p.aisle, x1 = p.inner, low = b.floor + wall.y, top = low + wall.x;
	agent_box(mb, Vector3(x0, low, front), Vector3(x1, top, p.partition), c.paint.lining, AgentTag::FIXED);
	agent_pane(mb, Vector3(x0, top, p.partition), Vector3(x1, top, p.partition), Vector3(x1, liner, p.partition), Vector3(x0, liner, p.partition),
			Vector3(0, 0, -1), trim);
}

Cabin cabin_of(const Bus &b, const Dictionary &in) {
	Cabin c;
	c.floor = b.floor;
	c.roof = b.cant;
	c.half_w = b.hw - b.wall;
	c.z_front = b.hl - b.wall;
	c.z_back = -b.hl + b.wall;
	c.side_top = b.sill;
	c.front_top = b.sill;
	c.back_top = b.rear_sill;
	c.doors = b.doors;
	c.paint = read_cabin_paint(in);
	return c;
}

// The lower body: walls with the arches and doorways, the dark strip along the bottom and the line under the windows, the
// panels at the ends under the windscreen and the rear window.
void lower(MeshBuilder &mb, const Bus &b, float pan, const Vector2 &skirt, const AgentTrim &trim) {
	Tub t;
	t.half_w = b.hw; t.wall = b.wall; t.y0 = b.clear; t.y1 = b.sill; t.z0 = -b.hl; t.z1 = b.hl; t.floor = b.floor;
	t.arch_r = b.arch_r; t.arch_top = b.arch_top; t.axles = b.g.axles; t.doors = b.doors; t.pan_half_w = pan;
	agent_tub(mb, t, trim);
	for (int side : { -1, 1 }) {
		const float x0 = side * b.hw, x1 = side * (b.hw + HEAVY_PROUD_M);
		const std::vector<Vector2> spans = side < 0 ? spans_between(-b.hl, b.hl, b.doors) : std::vector<Vector2>{ Vector2(-b.hl, b.hl) };
		for (const Vector2 &s : spans) {
			agent_arched_box(mb, std::min(x0, x1), std::max(x0, x1), b.clear, b.clear + skirt.x, s.x, s.y, b.g.axles, b.arch_r, b.arch_top, trim.blackout,
					AgentTag::FIXED);
			agent_box(mb, Vector3(x0, b.sill - skirt.y, s.x), Vector3(x1, b.sill, s.y), trim.blackout, AgentTag::FIXED);
		}
	}
	agent_box(mb, Vector3(-b.hw, b.clear, b.hl - b.wall), Vector3(b.hw, b.sill, b.hl), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-b.hw, b.clear, -b.hl), Vector3(b.hw, b.rear_sill, -b.hl + b.wall), AGENT_WHITE, AgentTag::PAINT);
}

// Pillars at the band's ends and as many between as keep every window no wider than the widest; none in a doorway, one at
// each of its edges.
std::vector<float> posts(const Bus &b, int side, float z0, float z1) {
	std::vector<float> out;
	const std::vector<Vector2> runs = side < 0 ? spans_between(z0, z1, b.doors) : std::vector<Vector2>{ Vector2(z0, z1) };
	for (const Vector2 &r : runs) {
		const int bays = std::max(1, int(std::ceil((r.y - r.x) / b.pitch)));
		for (int i = 0; i <= bays; i++) out.push_back(r.x + (r.y - r.x) * i / bays);
	}
	return out;
}

// One leaf of a door, from its hinge at `jamb` to `free`: the vertex stage swings what is marked with its hinge in UV.
void leaf(MeshBuilder &mb, const Bus &b, float jamb, float free, Limb limb, const AgentTrim &trim) {
	const float xd = -b.hw - HEAVY_PROUD_M, low = b.clear;
	const Vector2 hinge(xd, jamb), uvs[4] = { hinge, hinge, hinge, hinge };
	mb.quad(Vector3(xd, low, jamb), Vector3(xd, low, free), Vector3(xd, b.cant, free), Vector3(xd, b.cant, jamb), trim.glass, Vector3(-1, 0, 0), uvs, agent_tag(AgentTag::GLASS, limb));
	const size_t from = mb.uv.size();
	for (float z : { jamb, free })
		agent_box(mb, Vector3(xd - HEAVY_PROUD_M, low, z - DOOR_SPLIT_W), Vector3(-b.hw + HEAVY_PROUD_M, b.cant, z + DOOR_SPLIT_W), trim.blackout, AgentTag::FIXED, limb);
	for (size_t i = from; i < mb.uv.size(); i++) mb.uv[i] = hinge;
}

void band(MeshBuilder &mb, const Bus &b, const AgentTrim &trim) {
	const float z0 = -b.hl + HEAVY_PILLAR_M, z1 = b.hl - HEAVY_PILLAR_M, hp = HEAVY_PILLAR_M * 0.5f;
	for (int side : { -1, 1 }) {
		const float x = side * (b.hw - HEAVY_GLASS_INSET_M);
		const std::vector<Vector2> spans = side < 0 ? spans_between(z0, z1, b.doors) : std::vector<Vector2>{ Vector2(z0, z1) };
		for (const Vector2 &s : spans)
			agent_pane(mb, Vector3(x, b.sill, s.x), Vector3(x, b.sill, s.y), Vector3(x, b.cant, s.y), Vector3(x, b.cant, s.x), Vector3(side, 0, 0), trim);
		for (float z : posts(b, side, z0, z1))
			agent_box(mb, Vector3(side * (b.hw - HEAVY_PROUD_M), b.sill, z - hp), Vector3(side * (b.hw + HEAVY_PROUD_M), b.cant, z + hp), trim.blackout,
					AgentTag::FIXED);
	}
	// the doors' leaves on the kerb side: glazed from the bottom strip to the cant rail, split in the middle, each hung at its jamb
	for (const Vector2 &d : b.doors) {
		const float mid = (d.x + d.y) * 0.5f;
		leaf(mb, b, d.y, mid, Limb::DOOR, trim);
		leaf(mb, b, d.x, mid, Limb::DOOR_BACK, trim);
	}
}

// The windscreen between its corner pillars with the lit destination sign along its top, the rear window, the roof and
// its air conditioning.
void ends_and_roof(MeshBuilder &mb, const Bus &b, const Dictionary &d, const AgentTrim &trim) {
	const float zf = b.hl + HEAVY_PROUD_M, sx = b.hw - HEAVY_PILLAR_M, zb = -b.hl - HEAVY_PROUD_M;
	agent_pane(mb, Vector3(-sx, b.sill, zf), Vector3(sx, b.sill, zf), Vector3(sx, b.cant, zf), Vector3(-sx, b.cant, zf), Vector3(0, 0, 1), trim);
	agent_pane(mb, Vector3(-sx, b.rear_sill, zb), Vector3(sx, b.rear_sill, zb), Vector3(sx, b.cant, zb), Vector3(-sx, b.cant, zb), Vector3(0, 0, -1), trim);
	for (int side : { -1, 1 })
		for (float z : { b.hl, -b.hl + HEAVY_PILLAR_M })
			agent_box(mb, Vector3(side * sx, std::min(b.sill, b.rear_sill), z - HEAVY_PILLAR_M), Vector3(side * b.hw, b.cant, z), trim.blackout, AgentTag::FIXED);
	const Vector2 sign = agent_v2(d["sign_m"]); // height, in from each side
	agent_box(mb, Vector3(-b.hw + sign.y, b.cant - sign.x, zf), Vector3(b.hw - sign.y, b.cant, zf + HEAVY_PROUD_M), trim.blackout, AgentTag::SIGN);
	agent_box(mb, Vector3(-b.hw, b.cant, -b.hl), Vector3(b.hw, b.H, b.hl), AGENT_WHITE, AgentTag::PAINT);
	const Vector3 ac = agent_v3(d["ac_m"]); // length, width, height
	agent_box(mb, Vector3(-ac.y * 0.5f, b.H, -ac.x * 0.5f), Vector3(ac.y * 0.5f, b.H + ac.z, ac.x * 0.5f), trim.roof, AgentTag::FIXED);
}

} // namespace

int build_bus(MeshBuilder &mb, const Dictionary &d, int sides, const CabinLook *look, const AgentTrim &trim) {
	const Bus b = read_bus(d);
	const Dictionary in = d["interior"];
	Cabin c = cabin_of(b, in);
	int passengers = 0;
	float aisle = b.well_x; // the far level: the housings as deep as the wells
	std::vector<Vector2> plat[2];
	if (look) {
		Plan p = plan_of(b, in, c, *look);
		dash_for(c, p.driver, *look, p.inner - p.aisle);
		deck(mb, b, p, c);
		fittings(mb, b, p, in, c, *look, trim);
		p.seats.insert(p.seats.begin(), p.driver);
		passengers = agent_cabin(mb, c, p.seats, *look);
		aisle = p.aisle;
		plat[0] = p.platform[0];
		plat[1] = p.platform[1];
	} else {
		for (float ax : b.g.axles)
			for (std::vector<Vector2> &s : plat) s.push_back(Vector2(ax - b.arch_r, ax + b.arch_r));
	}
	housings(mb, b, plat, aisle, agent_hex(in["lining"]), trim);
	lower(mb, b, aisle, agent_v2(d["skirt_m"]), trim);
	band(mb, b, trim);
	ends_and_roof(mb, b, d, trim);
	const Vector2 mirror = agent_v2(d["rear_view_m"]); // glass bottom over the road, out of the body
	heavy_mirrors(mb, b.hw, b.hl, mirror.x, mirror.y, trim);
	heavy_lamps(mb, d["lamps_m"], b.W, b.L, trim);
	heavy_wheels(mb, b.g, sides, trim);
	return passengers;
}

} // namespace city
