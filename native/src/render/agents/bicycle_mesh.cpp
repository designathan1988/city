// A city bicycle with its rider (agents.json "bicycle"): two wheels, the diamond frame of struts from the bottom bracket
// to the seat and head tubes, the stays to the rear axle, the fork, the bars and the saddle. Parts in the model frame:
// +Z forward, +X the rider's left. The wheels turn with the same tag as any other vehicle's (agent_wheel).
#include "render/agents/agent_meshes.h"

using namespace godot;

namespace city {

namespace {
constexpr float STAY_X = 0.06f, FORK_X = 0.05f; // half the width of the rear stays and of the fork at the axle
constexpr float SADDLE_LONG = 0.26f, SADDLE_HALF_W = 0.08f, SADDLE_TOP = 0.04f, BAR_HALF = 0.28f, THIN = 0.8f;
constexpr float HELMET_TALL = 0.9f, HELMET_LONG = 1.2f;
} // namespace

int build_bicycle(MeshBuilder &mb, const Dictionary &d, int sides, const AgentTrim &trim) {
	const Vector2 tyre = agent_tyre(d["tyre"]);
	const float az = float(d["wheelbase_m"]) * 0.5f, r = tyre.x, tube = float(d["tube_m"]), saddle = float(d["saddle_m"]);
	const Vector2 bracket = agent_v2(d["bracket"]), head = agent_v2(d["head"]); // [z, y]
	const Vector3 bb(0, bracket.y, bracket.x), top(0, head.y, head.x), seat(0, saddle - SADDLE_TOP, bracket.x - float(d["seat_back_m"]));
	const Vector3 low(0, head.y - float(d["head_tube_m"]), head.x + float(d["head_rake_m"]));
	for (float z : { -az, az }) agent_spoked_wheel(mb, Vector3(0, r, z), r, tyre.y, sides, int(d["spokes"]), trim);
	agent_strut(mb, bb, seat, tube, tube, AGENT_WHITE, AgentTag::PAINT);
	agent_strut(mb, seat, top, tube, tube, AGENT_WHITE, AgentTag::PAINT);
	agent_strut(mb, bb, low, tube, tube, AGENT_WHITE, AgentTag::PAINT);
	agent_strut(mb, low, top, tube, tube, AGENT_WHITE, AgentTag::PAINT);
	for (float side : { -1.f, 1.f }) {
		const Vector3 rear(side * STAY_X, r, -az), fork(side * FORK_X, r, az);
		agent_strut(mb, bb, rear, tube * THIN, tube * THIN, AGENT_WHITE, AgentTag::PAINT);
		agent_strut(mb, seat, rear, tube * THIN, tube * THIN, AGENT_WHITE, AgentTag::PAINT);
		agent_strut(mb, low, fork, tube * THIN, tube * THIN, trim.blackout, AgentTag::FIXED);
	}
	agent_box(mb, Vector3(-SADDLE_HALF_W, saddle - SADDLE_TOP, seat.z - SADDLE_LONG * 0.5f), Vector3(SADDLE_HALF_W, saddle, seat.z + SADDLE_LONG * 0.5f),
			trim.blackout, AgentTag::FIXED);
	agent_strut(mb, Vector3(-BAR_HALF, head.y, head.x), Vector3(BAR_HALF, head.y, head.x), tube, tube, trim.blackout, AgentTag::FIXED);
	const Dictionary rack = d["rack"];
	const Vector2 at = agent_v2(rack["at"]), lens = agent_v2(rack["lens_m"]);
	const Vector3 size = agent_v3(rack["size_m"]);
	agent_box(mb, Vector3(-size.x * 0.5f, at.y, at.x - size.z * 0.5f), Vector3(size.x * 0.5f, at.y + size.y, at.x + size.z * 0.5f), trim.blackout, AgentTag::FIXED);
	agent_box(mb, Vector3(-lens.x * 0.5f, at.y + size.y, at.x - size.z * 0.5f), Vector3(lens.x * 0.5f, at.y + size.y + lens.y, at.x - size.z * 0.5f + lens.y), trim.tail, AgentTag::TAIL);
	const float helmet = float(d["helmet_r"]);
	agent_helmet(mb, d["rider"], Vector3(helmet, helmet * HELMET_TALL, helmet * HELMET_LONG));
	return 0;
}

} // namespace city
