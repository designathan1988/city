// A city bicycle with its rider (agents.json "bicycle"): two wheels, the diamond frame of struts from the bottom bracket
// to the seat and head tubes, the stays to the rear axle, the fork, the bars and the saddle. Parts in the model frame:
// +Z forward, +X the rider's left. The wheels turn with the same tag as any other vehicle's (agent_wheel).
#include "render/agents/agent_meshes.h"

using namespace godot;

namespace city {

namespace {
} // namespace

int build_bicycle(MeshBuilder &mb, const DataValue &d, int sides, const AgentTrim &trim) {
	const BicycleShape &K = trim.shapes->bicycle_mesh;
	const Vector2 tyre = agent_tyre(d["tyre"]);
	const float az = d["wheelbase_m"].f() * 0.5f, r = tyre.x, tube = d["tube_m"].f(), saddle = d["saddle_m"].f();
	const Vector2 bracket = agent_v2(d["bracket"]), head = agent_v2(d["head"]); // [z, y]
	const Vector3 bb(0, bracket.y, bracket.x), top(0, head.y, head.x), seat(0, saddle - K.saddle_top, bracket.x - d["seat_back_m"].f());
	const Vector3 low(0, head.y - d["head_tube_m"].f(), head.x + d["head_rake_m"].f());
	for (float z : { -az, az }) agent_spoked_wheel(mb, Vector3(0, r, z), r, tyre.y, sides, d["spokes"].whole(), trim);
	agent_strut(mb, bb, seat, tube, tube, AGENT_WHITE, AgentTag::PAINT);
	agent_strut(mb, seat, top, tube, tube, AGENT_WHITE, AgentTag::PAINT);
	agent_strut(mb, bb, low, tube, tube, AGENT_WHITE, AgentTag::PAINT);
	agent_strut(mb, low, top, tube, tube, AGENT_WHITE, AgentTag::PAINT);
	for (float side : { -1.f, 1.f }) {
		const Vector3 rear(side * K.stay_x, r, -az), fork(side * K.fork_x, r, az);
		agent_strut(mb, bb, rear, tube * K.thin, tube * K.thin, AGENT_WHITE, AgentTag::PAINT);
		agent_strut(mb, seat, rear, tube * K.thin, tube * K.thin, AGENT_WHITE, AgentTag::PAINT);
		agent_strut(mb, low, fork, tube * K.thin, tube * K.thin, trim.blackout, AgentTag::FIXED);
	}
	agent_box(mb, Vector3(-K.saddle_half_w, saddle - K.saddle_top, seat.z - K.saddle_long * 0.5f), Vector3(K.saddle_half_w, saddle, seat.z + K.saddle_long * 0.5f),
			trim.blackout, AgentTag::FIXED);
	agent_strut(mb, Vector3(-K.bar_half, head.y, head.x), Vector3(K.bar_half, head.y, head.x), tube, tube, trim.blackout, AgentTag::FIXED);
	const DataValue rack = d["rack"];
	const Vector2 at = agent_v2(rack["at"]), lens = agent_v2(rack["lens_m"]);
	const Vector3 size = agent_v3(rack["size_m"]);
	agent_box(mb, Vector3(-size.x * 0.5f, at.y, at.x - size.z * 0.5f), Vector3(size.x * 0.5f, at.y + size.y, at.x + size.z * 0.5f), trim.blackout, AgentTag::FIXED);
	agent_box(mb, Vector3(-lens.x * 0.5f, at.y + size.y, at.x - size.z * 0.5f), Vector3(lens.x * 0.5f, at.y + size.y + lens.y, at.x - size.z * 0.5f + lens.y), trim.tail, AgentTag::TAIL);
	return 0;
}

} // namespace city
