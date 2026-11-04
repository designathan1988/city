// A cooked person measured into an articulated body (person_rig.h): weights, centres, inertia, pivots, axes and skin points.
#include "sim/physics/person_rig.h"

#include <algorithm>
#include <limits>

namespace city::phys {

namespace {
constexpr float NO_LIMIT = std::numeric_limits<float>::max(); // farther than any distance on a body
constexpr float MIN_WEIGHT_SUM = 1e-6f; // a body or a joint with less skin weight than this has no skin
constexpr float DOMINANT_WEIGHT = 0.5f; // a vertex is on a body's surface from this share of its weight
constexpr float SHORT_AXIS = 0.1f; // a joint's bone this close to the reference axis takes the other one
const V3 DOWN(0, -1, 0), FORWARD(0, 0, 1), UP(0, 1, 0);

using Weights = std::vector<std::vector<float>>; // per body and vertex: the share of that vertex's skin the body moves

V3 at(const SkinView &s, int v) { return V3(s.positions[v * 3], s.positions[v * 3 + 1], s.positions[v * 3 + 2]); }

// The second moment of the weighted points about `c`: sum w (|r|^2 E - r r^T) / sum w.
M3 spread(const SkinView &s, const std::vector<float> &w, const V3 &c, float total) {
	M3 m;
	for (float &x : m.m) x = 0;
	for (int v = 0; v < s.vertices; v++) {
		if (w[size_t(v)] <= 0) continue;
		const V3 r = at(s, v) - c;
		const float k = w[size_t(v)] / total, rr = dot(r, r);
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++) m.at(i, j) += k * ((i == j ? rr : 0.f) - r[i] * r[j]);
	}
	return m;
}

// `count` skin points of a body spread over it: the farthest from the centre first, then each one farthest from those chosen.
std::vector<V3> sample(const SkinView &s, const std::vector<float> &w, const V3 &c, int count) {
	std::vector<int> cand;
	for (int v = 0; v < s.vertices; v++)
		if (w[size_t(v)] >= DOMINANT_WEIGHT) cand.push_back(v);
	if (cand.empty())
		for (int v = 0; v < s.vertices; v++)
			if (w[size_t(v)] > 0) cand.push_back(v);
	std::vector<V3> out;
	std::vector<float> near_d(cand.size(), NO_LIMIT);
	V3 last = c;
	for (int k = 0; k < count && !cand.empty(); k++) {
		size_t best = 0;
		float far = -1;
		for (size_t i = 0; i < cand.size(); i++) {
			const V3 p = at(s, cand[i]);
			if (k > 0) near_d[i] = std::min(near_d[i], dot(p - last, p - last));
			const float score = k == 0 ? dot(p - c, p - c) : near_d[i];
			if (score > far) {
				far = score;
				best = i;
			}
		}
		last = at(s, cand[best]);
		out.push_back(last - c);
	}
	return out;
}

// Each body's centre, inertia and skin points, from the weights.
bool measure_bodies(const SkinView &skin, const RigDef &def, const Weights &weight, PersonRig &rig, std::string &error) {
	const int nb = int(def.bodies.size()), nv = skin.vertices;
	rig.proto.bodies.resize(size_t(nb));
	rig.centre.assign(size_t(nb), V3());
	rig.proto.points.reserve(size_t(nb * def.contact_points));
	for (int b = 0; b < nb; b++) {
		const std::vector<float> &w = weight[size_t(b)];
		float total = 0;
		V3 c;
		for (int v = 0; v < nv; v++) {
			total += w[size_t(v)];
			c += at(skin, v) * w[size_t(v)];
		}
		if (total < MIN_WEIGHT_SUM) {
			error = "corpo " + std::to_string(b) + " sem pele";
			return false;
		}
		c = c * (1.f / total);
		rig.centre[size_t(b)] = c;
		const float mass = def.bodies[size_t(b)].mass;
		M3 inertia = spread(skin, w, c, total);
		for (float &x : inertia.m) x *= mass;
		for (int i = 0; i < 3; i++) inertia.at(i, i) += mass * def.min_gyration_m * def.min_gyration_m;
		Body &body = rig.proto.bodies[size_t(b)];
		body.x = body.x_prev = c;
		body.inv_mass = 1.f / mass;
		body.inertia = inertia;
		body.inv_inertia = inverse(inertia);
		body.inv_world = body.inv_inertia;
		for (const V3 &r : sample(skin, w, c, def.contact_points)) rig.proto.points.push_back({ b, r });
	}
	return true;
}

// The pivot of every joint: the mean of the skin weighted by both bodies' shares.
bool measure_pivots(const SkinView &skin, const RigDef &def, const Weights &weight, std::vector<V3> &pivot, std::string &error) {
	pivot.assign(def.joints.size(), V3());
	for (size_t j = 0; j < def.joints.size(); j++) {
		const JointDef &d = def.joints[j];
		float total = 0;
		V3 p;
		for (int v = 0; v < skin.vertices; v++) {
			const float k = weight[size_t(d.parent)][size_t(v)] * weight[size_t(d.child)][size_t(v)];
			total += k;
			p += at(skin, v) * k;
		}
		if (total < MIN_WEIGHT_SUM) {
			error = "junta " + std::to_string(j) + " sem pele em comum";
			return false;
		}
		pivot[j] = p * (1.f / total);
	}
	return true;
}

// The joints: the bone's direction at bind, its neutral direction and the offset to it, its axes.
bool measure_joints(const SkinView &skin, const RigDef &def, const Weights &weight, PersonRig &rig, std::string &error) {
	std::vector<V3> pivot;
	if (!measure_pivots(skin, def, weight, pivot, error)) return false;
	std::vector<int> joint_of(def.bodies.size(), -1); // the joint whose child is a body
	for (size_t j = 0; j < def.joints.size(); j++) joint_of[size_t(def.joints[j].child)] = int(j);
	for (size_t j = 0; j < def.joints.size(); j++) {
		const JointDef &d = def.joints[j];
		V3 end;
		if (d.distal >= 0) {
			if (joint_of[size_t(d.distal)] < 0) {
				error = "junta " + std::to_string(j) + ": o corpo distal não tem junta";
				return false;
			}
			end = pivot[size_t(joint_of[size_t(d.distal)])];
		} else {
			float far = -1;
			for (int v = 0; v < skin.vertices; v++) {
				if (weight[size_t(d.child)][size_t(v)] < DOMINANT_WEIGHT) continue;
				const V3 r = at(skin, v) - pivot[j];
				if (dot(r, r) > far) {
					far = dot(r, r);
					end = at(skin, v);
				}
			}
		}
		const V3 bone = unit(end - pivot[j]);
		V3 ey = bone;
		if (d.neutral == Neutral::Hanging) ey = DOWN;
		else if (d.neutral == Neutral::Collinear && joint_of[size_t(d.parent)] >= 0) ey = unit(pivot[j] - pivot[size_t(joint_of[size_t(d.parent)])]);
		V3 ex = cross(ey, d.ref_up ? UP : FORWARD);
		if (length(ex) < SHORT_AXIS) ex = cross(ey, d.ref_up ? FORWARD : UP);
		ex = unit(ex);
		Joint jt;
		jt.parent = d.parent;
		jt.child = d.child;
		jt.r_parent = pivot[j] - rig.centre[size_t(d.parent)];
		jt.r_child = pivot[j] - rig.centre[size_t(d.child)];
		jt.ex = ex;
		jt.ey = ey;
		jt.ez = cross(ex, ey);
		jt.offset = shortest_arc(ey, bone);
		jt.flex_lo = d.flex_lo;
		jt.flex_hi = d.flex_hi;
		jt.lat_lo = d.lat_lo;
		jt.lat_hi = d.lat_hi;
		jt.twist_lo = d.twist_lo;
		jt.twist_hi = d.twist_hi;
		rig.proto.joints.push_back(jt);
	}
	return true;
}
} // namespace

bool PersonRig::build(const SkinView &skin, const RigDef &def, std::string &error) {
	const int nb = int(def.bodies.size()), nv = skin.vertices;
	bone_body.assign(size_t(def.bone_count), -1);
	for (int b = 0; b < nb; b++)
		for (const int bone : def.bodies[size_t(b)].bones) {
			if (bone < 0 || bone >= def.bone_count) {
				error = "osso " + std::to_string(bone) + " fora da contagem";
				return false;
			}
			bone_body[size_t(bone)] = b;
		}
	Weights weight(size_t(nb), std::vector<float>(size_t(nv), 0.f));
	for (int v = 0; v < nv; v++)
		for (int k = 0; k < 4; k++) {
			const int bone = skin.bones[v * 4 + k];
			if (bone >= 0 && bone < def.bone_count && bone_body[size_t(bone)] >= 0) weight[size_t(bone_body[size_t(bone)])][size_t(v)] += skin.weights[v * 4 + k];
		}
	proto = Articulation();
	if (!measure_bodies(skin, def, weight, *this, error) || !measure_joints(skin, def, weight, *this, error)) return false;
	descendants.assign(size_t(nb), {});
	for (int b = 0; b < nb; b++)
		for (int a = b; a >= 0; a = def.bodies[size_t(a)].parent) descendants[size_t(a)].push_back(b); // b is below a and every ancestor of b
	return true;
}

} // namespace city::phys
