// Bus, lorry and motorbike (Roadcraft render/heavyBody.ts and vehicleModels.ts, reduced to boxes): painted shells
// with wheel-arch cut-outs, a glazed window band with pillars, windscreens, lamps and wheels; the motorbike carries
// its rider.
#include "render/agents/agent_meshes.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>

using namespace godot;

namespace city {

namespace {

constexpr float ARCH_PAD_M = 0.07f, WHEEL_INSET_M = 0.2f, GLASS_INSET_M = 0.02f, PILLAR_W = 0.12f, PROUD_M = 0.01f;
constexpr float SCREEN_LOW_M = 0.95f, SCREEN_TOP_PAD_M = 0.32f, SCREEN_SIDE_M = 0.12f, BLIND_M = 0.22f, BLIND_W = 0.8f;
constexpr float ROOF_INSET_M = 0.1f, ROOF_T = 0.06f, POD_L = 2.4f, POD_W = 0.7f, POD_H = 0.25f;
constexpr float LAMP_Y_M = 0.62f, LAMP_W = 0.32f, LAMP_H = 0.14f, LAMP_X_PAD = 0.35f, TAIL_Y_M = 1.0f, SKIRT_M = 0.16f;
constexpr float RIDER_TORSO_M = 0.6f, RIDER_ARMS_LOW_M = 0.43f, RIDER_ARMS_HIGH_M = 0.55f;
// motorbike proportions (shares of the axle distance or of the body half width)
constexpr float MOTO_TAIL = 0.8f, MOTO_NOSE = 0.7f, MOTO_SEAT_W = 0.8f, MOTO_BAR_W = 2.f, MOTO_BAR_Z = 0.6f, MOTO_BAR_H = 1.5f;
constexpr float MOTO_LAMP_W = 0.4f, MOTO_LAMP_D = 0.03f, RIDER_LEGS_W = 1.2f, RIDER_TORSO_W = 1.1f, RIDER_ARMS_W = 1.3f;
constexpr float RIDER_BACK = 0.3f, RIDER_KNEE = 0.15f, RIDER_HEAD_BACK = 0.1f, HELMET_TALL = 1.1f, HELMET_LONG = 1.15f;
constexpr float CHASSIS_SHARE = 0.8f; // lorry chassis rails against the body width
constexpr float CAB_GLASS_LOW = 0.1f, CAB_GLASS_PAD = 0.12f, CHASSIS_Y = 0.55f, BUMPER_H = 0.3f, POST_W = 0.08f;
// bus doors on the kerb side (-X) [front - 1.5, front - 0.3] and [-0.9, 0.3] (heavyBody.ts), seam, rounded roof ends
constexpr float DOOR1_FROM = 1.5f, DOOR1_TO = 0.3f, DOOR2_FROM = -0.9f, DOOR2_TO = 0.3f, DOOR_SPLIT_W = 0.04f, SEAM_H = 0.05f;
constexpr float ROOF_END_M = 0.35f, ROOF_END_DROP = 0.12f, MIRROR_ARM = 0.35f, MIRROR_W = 0.08f, MIRROR_H = 0.32f, MIRROR_D = 0.12f;
constexpr float MIRROR_Y_SHARE = 0.62f, MIRROR_Z_BACK = 0.25f;
// lorry: steps, mudguards, side guards, fuel tank, rear door seam and handles
constexpr float STEP_H = 0.06f, STEP_D = 0.35f, STEP_OUT = 0.06f, GUARD_PAD = 0.06f, GUARD_T = 0.05f, SIDE_GUARD_H = 0.25f;
constexpr float SIDE_GUARD_Y = 0.45f, TANK_L = 1.1f, TANK_H = 0.5f, TANK_D = 0.45f, TANK_Y = 0.5f, HANDLE_W = 0.05f, HANDLE_H = 0.9f;
constexpr float HANDLE_Y = 1.6f, HANDLE_X = 0.15f, CAB_DOOR_SEAM = 0.03f;

std::vector<float> axles_of(const Dictionary &d, float L) {
	std::vector<float> zs;
	Array a = d["axles"];
	for (int i = 0; i < a.size(); i++) zs.push_back(float(a[i]) * L);
	return zs;
}

void wheels(MeshBuilder &mb, const std::vector<float> &zs, float W, float r, float ww, int sides, const AgentTrim &trim) {
	for (float z : zs)
		for (int side : { -1, 1 }) agent_wheel(mb, Vector3(side * (W * 0.5f - WHEEL_INSET_M), r, z), r, ww, sides, side, trim);
}

void lamps(MeshBuilder &mb, float W, float L, float tail_y, const AgentTrim &trim) {
	for (int side : { -1, 1 }) {
		float x = side * (W * 0.5f - LAMP_X_PAD);
		agent_box(mb, Vector3(x - LAMP_W * 0.5f, LAMP_Y_M, L * 0.5f - PROUD_M), Vector3(x + LAMP_W * 0.5f, LAMP_Y_M + LAMP_H, L * 0.5f + PROUD_M),
				trim.headlamp, AgentTag::HEAD);
		agent_box(mb, Vector3(x - LAMP_W * 0.5f, tail_y, -L * 0.5f - PROUD_M), Vector3(x + LAMP_W * 0.5f, tail_y + LAMP_H, -L * 0.5f + PROUD_M),
				trim.tail, AgentTag::TAIL);
	}
}

float ww_of(const Dictionary &d) { return float(d["wheel_w"]); }

void mirrors(MeshBuilder &mb, float hw, float z, float y, const AgentTrim &trim) {
	for (int side : { -1, 1 }) {
		float x0 = side * hw, x1 = side * (hw + MIRROR_ARM);
		agent_box(mb, Vector3(std::min(x0, x1), y + MIRROR_H - MIRROR_W, z - MIRROR_W * 0.5f), Vector3(std::max(x0, x1), y + MIRROR_H, z + MIRROR_W * 0.5f),
				trim.blackout, AgentTag::FIXED);
		agent_box(mb, Vector3(x1 - MIRROR_W * 0.5f, y, z - MIRROR_D * 0.5f), Vector3(x1 + MIRROR_W * 0.5f, y + MIRROR_H, z + MIRROR_D * 0.5f), trim.blackout,
				AgentTag::FIXED);
	}
}

} // namespace

void build_bus(MeshBuilder &mb, const Dictionary &d, int sides, const AgentTrim &trim) {
	const float L = float(d["length"]), W = float(d["width"]), H = float(d["height"]), r = float(d["wheel"]);
	const float clear = float(d["clearance"]), sill = float(d["sill"]), cant = H - float(d["cant"]), pitch = float(d["pillar_m"]);
	const float hw = W * 0.5f, hl = L * 0.5f;
	std::vector<float> zs = axles_of(d, L);
	agent_arched_box(mb, hw, clear, sill, L, zs, r + ARCH_PAD_M, 2 * r + ARCH_PAD_M, AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-hw + PROUD_M, clear - PROUD_M, -hl + PROUD_M), Vector3(hw - PROUD_M, clear + SKIRT_M, hl - PROUD_M), trim.blackout, AgentTag::FIXED);
	agent_box(mb, Vector3(-hw + GLASS_INSET_M, sill, -hl + GLASS_INSET_M), Vector3(hw - GLASS_INSET_M, cant, hl - GLASS_INSET_M), trim.glass, AgentTag::GLASS);
	for (float z = -hl + pitch * 0.5f; z < hl - pitch * 0.5f; z += pitch)
		for (int side : { -1, 1 })
			agent_box(mb, Vector3(side * hw - PROUD_M, sill, z - PILLAR_W * 0.5f), Vector3(side * hw + PROUD_M, cant, z + PILLAR_W * 0.5f), AGENT_WHITE,
					AgentTag::PAINT);
	agent_box(mb, Vector3(-hw, cant, -hl), Vector3(hw, H - ROOF_T, hl), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-hw + ROOF_INSET_M, H - ROOF_T, -hl + ROOF_INSET_M), Vector3(hw - ROOF_INSET_M, H, hl - ROOF_INSET_M), trim.roof, AgentTag::FIXED);
	agent_box(mb, Vector3(-POD_W, H, -POD_L * 0.5f), Vector3(POD_W, H + POD_H, POD_L * 0.5f), trim.roof, AgentTag::FIXED);
	agent_box(mb, Vector3(-hw + SCREEN_SIDE_M, SCREEN_LOW_M, hl - PROUD_M), Vector3(hw - SCREEN_SIDE_M, H - SCREEN_TOP_PAD_M, hl + PROUD_M), trim.glass,
			AgentTag::GLASS);
	agent_box(mb, Vector3(-BLIND_W, H - SCREEN_TOP_PAD_M, hl - PROUD_M), Vector3(BLIND_W, H - SCREEN_TOP_PAD_M + BLIND_M, hl + PROUD_M), trim.headlamp,
			AgentTag::HEAD);
	for (float z0 : { hl - DOOR1_FROM, DOOR2_FROM }) {
		float z1 = z0 + (z0 > 0 ? DOOR1_FROM - DOOR1_TO : DOOR2_TO - DOOR2_FROM);
		agent_box(mb, Vector3(-hw - PROUD_M * 2, clear + SKIRT_M, z0), Vector3(-hw + PROUD_M, cant, z1), trim.glass, AgentTag::GLASS);
		agent_box(mb, Vector3(-hw - PROUD_M * 3, clear + SKIRT_M, (z0 + z1) * 0.5f - DOOR_SPLIT_W), Vector3(-hw + PROUD_M, cant, (z0 + z1) * 0.5f + DOOR_SPLIT_W),
				trim.blackout, AgentTag::FIXED);
	}
	agent_box(mb, Vector3(-hw - PROUD_M, sill - SEAM_H, -hl), Vector3(hw + PROUD_M, sill, hl), trim.blackout, AgentTag::FIXED);
	for (int end : { -1, 1 })
		agent_box(mb, Vector3(-hw + ROOF_INSET_M, H - ROOF_END_DROP, end > 0 ? hl - ROOF_END_M : -hl), Vector3(hw - ROOF_INSET_M, H, end > 0 ? hl : -hl + ROOF_END_M),
				AGENT_WHITE, AgentTag::PAINT);
	mirrors(mb, hw, hl - MIRROR_Z_BACK, H * MIRROR_Y_SHARE, trim);
	lamps(mb, W, L, TAIL_Y_M, trim);
	wheels(mb, zs, W, r, float(d["wheel_w"]), sides, trim);
}

void build_truck(MeshBuilder &mb, const Dictionary &d, int sides, const AgentTrim &trim) {
	const float L = float(d["length"]), W = float(d["width"]), H = float(d["height"]), r = float(d["wheel"]);
	const float hw = W * 0.5f, hl = L * 0.5f, cab_back = hl - L * float(d["cab_share"]);
	const float cab_bottom = float(d["cab_bottom"]), belt = float(d["belt"]), cab_top = float(d["cab_top"]), box_bottom = float(d["box_bottom"]);
	std::vector<float> zs = axles_of(d, L);
	agent_box(mb, Vector3(-hw * CHASSIS_SHARE, CHASSIS_Y, -hl), Vector3(hw * CHASSIS_SHARE, box_bottom, hl - PROUD_M), trim.blackout, AgentTag::FIXED);
	agent_box(mb, Vector3(-hw, cab_bottom, cab_back), Vector3(hw, belt, hl), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-hw + GLASS_INSET_M, belt, cab_back + CAB_GLASS_PAD), Vector3(hw - GLASS_INSET_M, cab_top - CAB_GLASS_LOW, hl - GLASS_INSET_M),
			trim.glass, AgentTag::GLASS);
	agent_box(mb, Vector3(-hw, belt, cab_back), Vector3(hw, cab_top, cab_back + CAB_GLASS_PAD), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-hw, cab_top - CAB_GLASS_LOW, cab_back), Vector3(hw, cab_top, hl), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-hw, cab_bottom - BUMPER_H, hl - BUMPER_H), Vector3(hw, cab_bottom, hl + PROUD_M), trim.grille, AgentTag::FIXED);
	float box_front = cab_back - ARCH_PAD_M * 2;
	agent_box(mb, Vector3(-hw, box_bottom, -hl), Vector3(hw, H, box_front), trim.truck_box, AgentTag::FIXED);
	for (float z : { -hl, box_front - POST_W })
		for (int side : { -1, 1 })
			agent_box(mb, Vector3(side * hw - PROUD_M - POST_W * (side > 0), box_bottom, z), Vector3(side * hw + PROUD_M + POST_W * (side < 0), H, z + POST_W),
					trim.chrome, AgentTag::FIXED);
	for (int side : { -1, 1 }) {
		float x = side * hw;
		agent_box(mb, Vector3(x - side * STEP_D * 0.5f - STEP_OUT, cab_bottom - BUMPER_H, hl - L * float(d["cab_share"]) * 0.5f),
				Vector3(x + STEP_OUT * side, cab_bottom - BUMPER_H + STEP_H, hl - BUMPER_H), trim.chrome, AgentTag::FIXED);
		agent_box(mb, Vector3(x - PROUD_M, cab_bottom, cab_back + CAB_GLASS_PAD * 2), Vector3(x + PROUD_M, belt, cab_back + CAB_GLASS_PAD * 2 + CAB_DOOR_SEAM), trim.blackout,
				AgentTag::FIXED);
		for (float z : zs) {
			float zr = r + GUARD_PAD;
			agent_box(mb, Vector3(x - side * (WHEEL_INSET_M + ww_of(d)), 2 * r + GUARD_PAD, z - zr), Vector3(x, 2 * r + GUARD_PAD + GUARD_T, z + zr), trim.blackout,
					AgentTag::FIXED);
		}
		agent_box(mb, Vector3(x - side * POST_W, SIDE_GUARD_Y, zs[1] + r + GUARD_PAD), Vector3(x, SIDE_GUARD_Y + SIDE_GUARD_H, box_front - r), trim.chrome, AgentTag::FIXED);
		agent_box(mb, Vector3(-HANDLE_X * side - HANDLE_W * 0.5f, HANDLE_Y, -hl - PROUD_M * 2), Vector3(-HANDLE_X * side + HANDLE_W * 0.5f, HANDLE_Y + HANDLE_H, -hl),
				trim.chrome, AgentTag::FIXED);
	}
	agent_box(mb, Vector3(hw - TANK_D, TANK_Y, zs[0] - r - GUARD_PAD - TANK_L - TANK_L), Vector3(hw, TANK_Y + TANK_H, zs[0] - r - GUARD_PAD - TANK_L), trim.chrome,
			AgentTag::FIXED);
	agent_box(mb, Vector3(-DOOR_SPLIT_W * 0.5f, box_bottom, -hl - PROUD_M * 2), Vector3(DOOR_SPLIT_W * 0.5f, H, -hl), trim.blackout, AgentTag::FIXED);
	mirrors(mb, hw, hl - MIRROR_Z_BACK, belt, trim);
	lamps(mb, W, L, box_bottom, trim);
	wheels(mb, zs, W, r, float(d["wheel_w"]), sides, trim);
}

void build_moto(MeshBuilder &mb, const Dictionary &d, int sides, const AgentTrim &trim) {
	const float r = float(d["wheel"]), ww = float(d["wheel_w"]), az = float(d["axle_z"]), seat = float(d["rider_seat_y"]), helmet = float(d["helmet_r"]);
	Array b = d["body"];
	const float bw = float(b[0]) * 0.5f, by0 = float(b[1]), by1 = float(b[2]);
	const float tail = -az * MOTO_TAIL, nose = az * MOTO_NOSE, lamp_w = bw * MOTO_LAMP_W;
	for (float z : { -az, az }) agent_wheel(mb, Vector3(0, r, z), r, ww, sides, 1, trim);
	agent_box(mb, Vector3(-bw, by0, tail), Vector3(bw, by1, nose), AGENT_WHITE, AgentTag::PAINT);
	agent_box(mb, Vector3(-bw * MOTO_SEAT_W, by1, -az * MOTO_NOSE), Vector3(bw * MOTO_SEAT_W, by1 + PILLAR_W, 0), trim.blackout, AgentTag::FIXED);
	agent_box(mb, Vector3(-bw * MOTO_BAR_W, by1 + PILLAR_W, az * MOTO_BAR_Z), Vector3(bw * MOTO_BAR_W, by1 + PILLAR_W * MOTO_BAR_H, nose), trim.blackout,
			AgentTag::FIXED);
	agent_box(mb, Vector3(-lamp_w, by1 - LAMP_H, nose), Vector3(lamp_w, by1, nose + MOTO_LAMP_D), trim.headlamp, AgentTag::HEAD);
	agent_box(mb, Vector3(-lamp_w, by1 - LAMP_H, tail - MOTO_LAMP_D), Vector3(lamp_w, by1, tail), trim.tail, AgentTag::TAIL);
	// rider: legs, torso, arms reaching the bars, helmet in the bike's paint
	agent_box(mb, Vector3(-bw * RIDER_LEGS_W, by0, -az * RIDER_BACK), Vector3(bw * RIDER_LEGS_W, seat, az * RIDER_KNEE), trim.rider_legs, AgentTag::FIXED);
	agent_box(mb, Vector3(-bw * RIDER_TORSO_W, seat, -az * RIDER_BACK), Vector3(bw * RIDER_TORSO_W, seat + RIDER_TORSO_M, 0), trim.rider, AgentTag::FIXED);
	agent_box(mb, Vector3(-bw * RIDER_ARMS_W, seat + RIDER_ARMS_LOW_M, 0), Vector3(bw * RIDER_ARMS_W, seat + RIDER_ARMS_HIGH_M, az * MOTO_BAR_Z), trim.rider,
			AgentTag::FIXED);
	mb.blob(Vector3(0, seat + RIDER_TORSO_M + helmet, -az * RIDER_HEAD_BACK), Vector3(helmet, helmet * HELMET_TALL, helmet * HELMET_LONG), AGENT_WHITE,
			agent_tag(AgentTag::PAINT));
}

} // namespace city
