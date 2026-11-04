// City bus (Roadcraft 4.0 render/vehicleModels.ts buildBusModel, in boxes and panes): a hollow low-floor body whose
// side walls are cut over the wheels and open at the two doors on the kerb side (-X), with the floor pan under it; a
// glazed band of panes between black pillars, the windscreen between its corner pillars, a rear window and glazed door
// leaves; the roof with its air-conditioning pod. Inside (agent_cabin): the driver's cab (seat, wheel, dashboard,
// partition), the wheel housings standing in the low floor with the seats over them raised onto them, pairs of seats
// either side of the aisle and a row of five at the back, the stanchions and ceiling rails.
#include "render/agents/agent_meshes.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>

using namespace godot;

namespace city {

namespace {

constexpr float ROOF_INSET_M = 0.1f, ROOF_T = 0.06f, POD_L = 2.4f, POD_W = 0.7f, POD_H = 0.25f;
constexpr float SCREEN_TOP_PAD_M = 0.32f, BLIND_M = 0.22f, BLIND_W = 0.8f, TAIL_Y_M = 1.0f, SKIRT_M = 0.16f;
constexpr float DOOR_SPLIT_W = 0.04f, SEAM_H = 0.05f, ROOF_END_M = 0.35f, ROOF_END_DROP = 0.12f;
constexpr float MIRROR_Y_SHARE = 0.62f, MIRROR_Z_BACK = 0.25f, PARTITION_THICK_M = 0.04f;
constexpr float POCKET_SHARE = 0.95f; // a wheel housing's half length against the arch radius
constexpr float WELL_GAP_M = 0.02f;   // the wheel wells clear of the wheels' inner faces

struct Bus {
	float L = 0, W = 0, H = 0, r = 0, clear = 0, sill = 0, cant = 0, floor = 0, wall = 0, hw = 0, hl = 0, arch_r = 0, arch_top = 0, pitch = 0;
	float well_x = 0; // the wheel wells' inner side: clear of the wheels
	float screen_low = 0, screen_top = 0, screen_side = 0, rear_low = 0, rear_top = 0, rear_side = 0;
	std::vector<float> axles;
	std::vector<Vector2> doors; // kerb side, z spans
};

// Where the seats go: the rows, the driver and the partition behind them, the wheel housings of each side (z spans).
struct Plan {
	std::vector<float> rows;
	std::vector<Vector2> housing[2]; // [0] the kerb side (-X), [1] the driver's (+X)
	float driver_z = 0, partition = 0, housing_top = 0, housing_d = 0, pitch = 0;
	Vector2 reach, room, clear;
};

Bus read_bus(const Dictionary &d) {
	Bus b;
	b.L = d["length"]; b.W = d["width"]; b.H = d["height"]; b.r = d["wheel"]; b.clear = d["clearance"]; b.sill = d["sill"];
	b.cant = b.H - float(d["cant"]); b.floor = d["floor"]; b.wall = d["wall_m"]; b.pitch = d["pillar_m"];
	b.hw = b.W * 0.5f; b.hl = b.L * 0.5f;
	b.axles = heavy_axles(d, b.L);
	b.arch_r = b.r + HEAVY_ARCH_PAD_M; b.arch_top = 2 * b.r + HEAVY_ARCH_PAD_M;
	b.well_x = b.hw - float(d["wheel_inset_m"]) - float(d["wheel_w"]) - WELL_GAP_M;
	const Vector3 screen = agent_v3(d["screen"]), rear = agent_v3(d["rear_window"]); // [base, gap under the cant rail, side]; [base, top, side]
	b.screen_low = screen.x; b.screen_top = b.cant - screen.y; b.screen_side = screen.z;
	b.rear_low = rear.x; b.rear_top = rear.y; b.rear_side = rear.z;
	for (const Variant &v : Array(d["doors"])) b.doors.push_back(agent_v2(v));
	return b;
}

int sided(int side) { return side < 0 ? 0 : 1; }

bool in_doorway(const Bus &b, const Plan &p, float z) {
	for (const Vector2 &d : b.doors)
		if (z > d.x - p.clear.x && z < d.y + p.clear.y) return true;
	return false;
}

// Somebody can sit in a row on this side: not in a doorway, not up against the driver's partition.
bool seated(const Bus &b, const Plan &p, int side, float z) { return side < 0 ? !in_doorway(b, p, z) : z + p.room.y <= p.partition; }

bool over(const Plan &p, float z, float axle, float pocket) { return z + p.reach.y > axle - pocket && z - p.reach.x < axle + pocket; }

std::vector<Vector2> merged(std::vector<Vector2> spans) {
	std::sort(spans.begin(), spans.end(), [](const Vector2 &a, const Vector2 &b) { return a.x < b.x; });
	std::vector<Vector2> out;
	for (const Vector2 &s : spans)
		if (!out.empty() && s.x <= out.back().y) out.back().y = std::max(out.back().y, s.y);
		else out.push_back(s);
	return out;
}

Plan plan_of(const Bus &b, const Dictionary &in) {
	Plan p;
	const Vector3 rows = agent_v3(in["rows_m"]); // first from the front, last from the back, pitch
	p.pitch = rows.z;
	for (int i = 0; b.hl - rows.x - i * rows.z >= -b.hl + rows.y; i++) p.rows.push_back(b.hl - rows.x - i * rows.z);
	p.reach = agent_v2(in["reach_m"]); p.room = agent_v2(in["sit_room_m"]); p.clear = agent_v2(in["door_clear_m"]);
	const Vector2 driver = agent_v2(in["driver_m"]);
	p.driver_z = b.hl - driver.x;
	p.partition = p.driver_z - agent_v3(in["partition_m"]).x;
	p.housing_top = b.arch_top + float(in["housing_pad_m"]);
	p.housing_d = 2 * (float(in["chair_place_m"]) + float(in["wall_gap_m"]));
	const float pocket = b.arch_r * POCKET_SHARE;
	for (int side : { -1, 1 }) {
		std::vector<Vector2> spans;
		for (float ax : b.axles) {
			Vector2 s(ax - pocket, ax + pocket); // the housing, and the rows of seats standing on it end to end
			for (float z : p.rows)
				if (over(p, z, ax, pocket) && seated(b, p, side, z)) s = Vector2(std::min(s.x, z - p.reach.x), std::max(s.y, z + p.reach.y));
			spans.push_back(Vector2(std::max(s.x, -b.hl + b.wall), std::min(s.y, b.hl - b.wall)));
		}
		p.housing[sided(side)] = merged(spans);
	}
	return p;
}

// Each housing: a block inboard of the wheel up to its top, a cap over the wheel, the well round the wheel dark.
void housings(MeshBuilder &mb, const Bus &b, const Plan &p, const Color &c, const AgentTrim &trim) {
	const float eps = HEAVY_PROUD_M * 0.5f;
	for (int side : { -1, 1 })
		for (const Vector2 &s : p.housing[sided(side)]) {
			const float in = side * (b.hw - b.wall - p.housing_d), well = side * b.well_x, out = side * (b.hw - b.wall);
			agent_box(mb, Vector3(in, b.floor, s.x), Vector3(well, p.housing_top, s.y), c, AgentTag::FIXED);
			agent_box(mb, Vector3(well, b.arch_top, s.x), Vector3(out, p.housing_top, s.y), c, AgentTag::FIXED);
			const float xw = well + side * eps, yc = b.arch_top - eps;
			mb.quad(Vector3(xw, b.clear, s.x), Vector3(xw, b.clear, s.y), Vector3(xw, yc, s.y), Vector3(xw, yc, s.x), trim.under, Vector3(side, 0, 0), nullptr,
					agent_tag(AgentTag::FIXED));
			mb.quad(Vector3(well, yc, s.x), Vector3(out, yc, s.x), Vector3(out, yc, s.y), Vector3(well, yc, s.y), trim.under, Vector3(0, -1, 0), nullptr,
					agent_tag(AgentTag::FIXED));
		}
}

// A seat on its side's housing stands on it; one whose legs or back would run into a housing's end is left out.
bool place(const Bus &b, const Plan &p, Seat &s) {
	const int side = s.x < 0 ? -1 : 1;
	if (!seated(b, p, side, s.z)) return false;
	for (const Vector2 &h : p.housing[sided(side)]) {
		if (s.z - p.reach.x >= h.x && s.z + p.reach.y <= h.y) {
			s.floor = p.housing_top;
			return true;
		}
		if (s.z + p.room.y > h.x && s.z - p.room.x < h.y) return false; // its legs, its back or itself in the housing
	}
	return true;
}

std::vector<Seat> seats_of(const Bus &b, const Plan &p, const Dictionary &in, const CabinLook &l) {
	std::vector<Seat> seats(1);
	seats[0].x = b.hw - agent_v2(in["driver_m"]).y;
	seats[0].z = p.driver_z;
	seats[0].floor = b.floor;
	seats[0].style = &l.cab;
	seats[0].driver = true;
	const float cw = in["chair_place_m"], inner = b.hw - b.wall - float(in["wall_gap_m"]);
	std::vector<float> xs;
	for (int side : { 1, -1 })
		for (int k = 0; k < 2; k++) xs.push_back(side * (inner - cw * (k + 0.5f)));
	seat_rows(seats, p.rows.front(), p.rows.back(), p.pitch, xs, b.floor, l.chair, [&](Seat &s) { return place(b, p, s); });
	const Vector2 back = agent_v2(in["back_row_m"]); // hip from the back, gap between seats
	const int n = in["back_row_seats"];
	std::vector<float> row;
	for (int k = 0; k < n; k++) row.push_back((k - (n - 1) * 0.5f) * (cw + back.y));
	seat_rows(seats, -b.hl + back.x, -b.hl + back.x, p.pitch, row, b.floor, l.chair);
	return seats;
}

// Stanchions by the aisle every few rows and at the doors, the ceiling rails over the aisle, the driver's partition.
void fittings(MeshBuilder &mb, const Bus &b, const Plan &p, const Dictionary &in, float top) {
	const Color pole = agent_hex(in["pole"]), lining = agent_hex(in["lining"]);
	const Vector3 pm = agent_v3(in["pole_m"]); // thickness, ahead of its row, rail under the liner
	const float t = pm.x * 0.5f, aisle = b.hw - b.wall - p.housing_d;
	const int every = in["pole_rows"];
	auto stanchion = [&](float x, float z) { agent_box(mb, Vector3(x - t, b.floor, z - t), Vector3(x + t, top, z + t), pole, AgentTag::FIXED); };
	for (size_t i = 0; i < p.rows.size(); i += size_t(every))
		for (int side : { -1, 1 })
			if (seated(b, p, side, p.rows[i])) stanchion(side * aisle, p.rows[i] + pm.y);
	for (const Vector2 &d : b.doors) stanchion(-aisle, (d.x + d.y) * 0.5f);
	for (int side : { -1, 1 })
		agent_box(mb, Vector3(side * aisle - t, top - pm.z - pm.x, p.rows.back()), Vector3(side * aisle + t, top - pm.z, p.rows.front()), pole, AgentTag::FIXED);
	const Vector3 wall = agent_v3(in["partition_m"]); // behind the driver's hip, width, height
	agent_box(mb, Vector3(b.hw - b.wall - wall.y, b.floor, p.partition - PARTITION_THICK_M), Vector3(b.hw - b.wall, b.floor + wall.z, p.partition), lining,
			AgentTag::FIXED);
}

int interior(MeshBuilder &mb, const Bus &b, const Plan &p, const Dictionary &in, const CabinLook &l) {
	Cabin c;
	c.floor = b.floor;
	c.roof = b.cant;
	c.half_w = b.hw - b.wall;
	c.z_front = b.hl - b.wall;
	c.z_back = -b.hl + b.wall;
	c.side_top = b.sill;
	c.front_top = b.screen_low;
	c.back_top = b.rear_low;
	const Vector3 dash = agent_v3(in["dash_m"]); // depth, height, width from the driver's side
	c.dash_d = dash.x; c.dash_h = dash.y; c.dash_w = dash.z;
	c.doors = b.doors;
	c.paint = read_cabin_paint(in);
	fittings(mb, b, p, in, c.roof - l.liner_drop_m);
	return agent_cabin(mb, c, seats_of(b, p, in, l), l);
}

// The lower body: walls, skirt and seam strips outside them, the panels at the ends.
void lower(MeshBuilder &mb, const Bus &b, const Plan &p, const AgentTrim &trim) {
	Tub t;
	t.half_w = b.hw; t.wall = b.wall; t.y0 = b.clear; t.y1 = b.sill; t.z0 = -b.hl; t.z1 = b.hl; t.floor = b.floor;
	t.arch_r = b.arch_r; t.arch_top = b.arch_top; t.axles = b.axles; t.doors = b.doors;
	t.pan_half_w = b.hw - b.wall - p.housing_d; // between the housings, clear of the wheels
	agent_tub(mb, t, trim);
	for (int side : { -1, 1 }) {
		const float x0 = side * b.hw, x1 = side * (b.hw + HEAVY_PROUD_M);
		agent_box(mb, Vector3(x0, b.clear - HEAVY_PROUD_M, -b.hl), Vector3(x1, b.clear + SKIRT_M, b.hl), trim.blackout, AgentTag::FIXED);
		const std::vector<Vector2> spans = side < 0 ? spans_between(-b.hl, b.hl, b.doors) : std::vector<Vector2>{ Vector2(-b.hl, b.hl) };
		for (const Vector2 &s : spans) agent_box(mb, Vector3(x0, b.sill - SEAM_H, s.x), Vector3(x1, b.sill, s.y), trim.blackout, AgentTag::FIXED);
	}
	agent_box(mb, Vector3(-b.hw, b.clear, b.hl - b.wall), Vector3(b.hw, b.screen_low, b.hl), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-b.hw, b.clear, -b.hl), Vector3(b.hw, b.rear_low, -b.hl + b.wall), AGENT_WHITE, AgentTag::PAINT);
	for (int side : { -1, 1 })
		agent_box(mb, Vector3(side * b.hw, b.rear_low, -b.hl), Vector3(side * (b.hw - b.rear_side), b.cant, -b.hl + b.wall), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-b.hw + b.rear_side, b.rear_top, -b.hl), Vector3(b.hw - b.rear_side, b.cant, -b.hl + b.wall), AGENT_WHITE, AgentTag::PAINT);
}

// Pillars at the band's ends and every pillar_m between, none across a doorway, one at each of its edges.
std::vector<float> posts(const Bus &b, int side, float z0, float z1) {
	std::vector<float> out;
	const int bays = std::max(1, int((z1 - z0) / b.pitch + 0.5f));
	for (int i = 0; i <= bays; i++) {
		const float z = z0 + (z1 - z0) * i / bays;
		bool door = false;
		for (const Vector2 &d : b.doors) door = door || (side < 0 && z > d.x - HEAVY_PILLAR_M && z < d.y + HEAVY_PILLAR_M);
		if (!door) out.push_back(z);
	}
	if (side < 0)
		for (const Vector2 &d : b.doors) out.insert(out.end(), { d.x, d.y });
	return out;
}

void band(MeshBuilder &mb, const Bus &b, const AgentTrim &trim) {
	const float z0 = -b.hl + b.wall, z1 = b.hl - HEAVY_PILLAR_M, hp = HEAVY_PILLAR_M * 0.5f;
	for (int side : { -1, 1 }) {
		const float x = side * (b.hw - HEAVY_GLASS_INSET_M);
		const std::vector<Vector2> spans = side < 0 ? spans_between(z0, z1, b.doors) : std::vector<Vector2>{ Vector2(z0, z1) };
		for (const Vector2 &s : spans)
			agent_pane(mb, Vector3(x, b.sill, s.x), Vector3(x, b.sill, s.y), Vector3(x, b.cant, s.y), Vector3(x, b.cant, s.x), Vector3(side, 0, 0), trim);
		for (float z : posts(b, side, z0, z1))
			agent_box(mb, Vector3(side * (b.hw - HEAVY_PROUD_M), b.sill, z - hp), Vector3(side * (b.hw + HEAVY_PROUD_M), b.cant, z + hp), trim.blackout,
					AgentTag::FIXED);
	}
	// the door leaves on the kerb side: glazed from the skirt to the cant rail, their frame and the rail at the sill
	const float xd = -b.hw - HEAVY_PROUD_M, low = b.clear + SKIRT_M;
	for (const Vector2 &d : b.doors) {
		agent_pane(mb, Vector3(xd, low, d.x), Vector3(xd, low, d.y), Vector3(xd, b.cant, d.y), Vector3(xd, b.cant, d.x), Vector3(-1, 0, 0), trim);
		for (float z : { d.x, (d.x + d.y) * 0.5f, d.y })
			agent_box(mb, Vector3(xd - HEAVY_PROUD_M, low, z - DOOR_SPLIT_W), Vector3(-b.hw + HEAVY_PROUD_M, b.cant, z + DOOR_SPLIT_W), trim.blackout,
					AgentTag::FIXED);
		agent_box(mb, Vector3(xd - HEAVY_PROUD_M, b.sill - SEAM_H, d.x), Vector3(xd, b.sill, d.y), trim.blackout, AgentTag::FIXED);
	}
}

// Windscreen between its corner pillars under a black header, the destination blind over it, the rear window.
void ends(MeshBuilder &mb, const Bus &b, const AgentTrim &trim) {
	const float zf = b.hl + HEAVY_PROUD_M, sx = b.hw - b.screen_side;
	agent_pane(mb, Vector3(-sx, b.screen_low, zf), Vector3(sx, b.screen_low, zf), Vector3(sx, b.screen_top, zf), Vector3(-sx, b.screen_top, zf),
			Vector3(0, 0, 1), trim);
	for (int side : { -1, 1 })
		agent_box(mb, Vector3(side * sx, b.screen_low, b.hl - HEAVY_PILLAR_M), Vector3(side * b.hw, b.cant, zf), trim.blackout, AgentTag::FIXED);
	agent_box(mb, Vector3(-sx, b.screen_top, b.hl - b.wall), Vector3(sx, b.cant, zf), trim.blackout, AgentTag::FIXED);
	agent_box(mb, Vector3(-BLIND_W, b.H - SCREEN_TOP_PAD_M, b.hl - HEAVY_PROUD_M), Vector3(BLIND_W, b.H - SCREEN_TOP_PAD_M + BLIND_M, zf), trim.headlamp,
			AgentTag::HEAD);
	const float zb = -b.hl - HEAVY_PROUD_M, rx = b.hw - b.rear_side;
	agent_pane(mb, Vector3(-rx, b.rear_low, zb), Vector3(rx, b.rear_low, zb), Vector3(rx, b.rear_top, zb), Vector3(-rx, b.rear_top, zb), Vector3(0, 0, -1),
			trim);
}

void roof(MeshBuilder &mb, const Bus &b, const AgentTrim &trim) {
	agent_box(mb, Vector3(-b.hw, b.cant, -b.hl), Vector3(b.hw, b.H - ROOF_T, b.hl), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-b.hw + ROOF_INSET_M, b.H - ROOF_T, -b.hl + ROOF_INSET_M), Vector3(b.hw - ROOF_INSET_M, b.H, b.hl - ROOF_INSET_M), trim.roof,
			AgentTag::FIXED);
	agent_box(mb, Vector3(-POD_W, b.H, -POD_L * 0.5f), Vector3(POD_W, b.H + POD_H, POD_L * 0.5f), trim.roof, AgentTag::FIXED);
	for (int end : { -1, 1 })
		agent_box(mb, Vector3(-b.hw + ROOF_INSET_M, b.H - ROOF_END_DROP, end > 0 ? b.hl - ROOF_END_M : -b.hl),
				Vector3(b.hw - ROOF_INSET_M, b.H, end > 0 ? b.hl : -b.hl + ROOF_END_M), AGENT_WHITE, AgentTag::PAINT);
}

} // namespace

int build_bus(MeshBuilder &mb, const Dictionary &d, int sides, const CabinLook *look, const AgentTrim &trim) {
	const Bus b = read_bus(d);
	const Dictionary in = d["interior"];
	const Plan p = plan_of(b, in);
	const int passengers = look ? interior(mb, b, p, in, *look) : 0;
	housings(mb, b, p, agent_hex(in["lining"]), trim); // seen through the wheel arches too: at both levels
	lower(mb, b, p, trim);
	band(mb, b, trim);
	ends(mb, b, trim);
	roof(mb, b, trim);
	heavy_mirrors(mb, b.hw, b.hl - MIRROR_Z_BACK, b.H * MIRROR_Y_SHARE, trim);
	heavy_lamps(mb, b.W, b.L, TAIL_Y_M, trim);
	heavy_wheels(mb, d, b.axles, b.W, sides, trim);
	return passengers;
}

} // namespace city
