// What the tests read of the falling bodies (ragdoll_world.h): one person of a variation dropped or thrown on flat ground and stepped
// the way the game steps it, with the solver's own measures: how far the joints were pulled apart, how far a limit was overshot, how
// deep a point went in the ground, when it came to rest, and what a step cost.
#include "render/effects/ragdoll_world.h"

#include "core/units.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float FRAME_S = 1.f / 60.f; // the frames the probe steps, as the game at 60 frames a second
constexpr double MICRO = 1e6;

class FlatGround : public phys::Ground {
public:
	float height(float, float) const override { return 0.f; }
};
} // namespace

Dictionary RagdollWorld::probe(int variation, float speed_ms, float up_ms, float drop_m, float seconds, int substeps) {
	Dictionary out;
	const phys::PersonRig *rig = rig_for(variation);
	if (!rig) {
		out["error"] = "sem rig para a variação";
		return out;
	}
	phys::Articulation art = rig->proto;
	art.params = params_.solver;
	if (substeps > 0) art.params.substeps = substeps;
	for (phys::Body &b : art.bodies) {
		b.x.y += drop_m;
		b.x_prev = b.x;
		b.v = phys::V3(speed_ms, up_ms, 0.f);
	}
	art.refresh();
	const FlatGround ground;
	phys::StepCost cost;
	const int frames = int(seconds / FRAME_S);
	float gap = 0, over = 0, deepest = 0, rest_s = -1.f, us_max = 0;
	std::vector<float> joint_gap(art.joints.size(), 0.f), joint_over(art.joints.size(), 0.f);
	std::vector<int> joint_part(art.joints.size(), 0);
	double us_sum = 0;
	for (int f = 0; f < frames; f++) {
		const auto t0 = std::chrono::steady_clock::now();
		art.step(FRAME_S, ground, cost);
		const float us = float(std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count() * MICRO);
		us_sum += double(us);
		us_max = std::max(us_max, us);
		for (size_t j = 0; j < art.joints.size(); j++) {
			int part = 0;
			const float o = art.overshoot_of(j, part);
			joint_gap[j] = std::max(joint_gap[j], art.gap_of(j));
			if (o > joint_over[j]) {
				joint_over[j] = o;
				joint_part[j] = part;
			}
			gap = std::max(gap, joint_gap[j]);
			over = std::max(over, joint_over[j]);
		}
		for (const phys::ContactPoint &p : art.points) {
			const phys::Body &b = art.bodies[size_t(p.body)];
			deepest = std::max(deepest, -(b.x + b.q.rotate(p.r)).y);
		}
		if (rest_s < 0 && art.quiet_s >= params_.rest_after_s) rest_s = float(f + 1) * FRAME_S;
	}
	float top = 0;
	for (const phys::ContactPoint &p : art.points) {
		const phys::Body &b = art.bodies[size_t(p.body)];
		top = std::max(top, (b.x + b.q.rotate(p.r)).y);
	}
	Array ja, jo, jp;
	for (size_t j = 0; j < art.joints.size(); j++) {
		ja.push_back(joint_gap[j]);
		jo.push_back(joint_over[j] * RAD_TO_DEG);
		jp.push_back(joint_part[j]);
	}
	out["joint_gap_m"] = ja;
	out["joint_overshoot_deg"] = jo;
	out["joint_part"] = jp;
	out["frames"] = frames;
	out["bodies"] = int(art.bodies.size());
	out["joints"] = int(art.joints.size());
	out["points"] = int(art.points.size());
	out["gap_m"] = gap;
	out["overshoot_deg"] = over * RAD_TO_DEG;
	out["penetration_m"] = deepest;
	out["rest_s"] = rest_s;
	out["top_m"] = top;
	out["pelvis_x"] = art.bodies[0].x.x;
	out["pelvis_y"] = art.bodies[0].x.y;
	out["us_avg"] = frames > 0 ? float(us_sum / double(frames)) : 0.f;
	out["us_max"] = us_max;
	out["contacts_avg"] = frames > 0 ? float(cost.contacts) / float(frames) : 0.f;
	out["fastest"] = art.fastest();
	return out;
}

} // namespace city
