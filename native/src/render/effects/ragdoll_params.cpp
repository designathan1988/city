// data/render/ragdoll.json into RagdollParams (ragdoll_params.h), strictly: a missing key or an unknown name is reported.
#include "render/effects/ragdoll_params.h"

#include "core/units.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <cmath>
#include <map>

using namespace godot;

namespace city {

namespace {
constexpr float MASS_SUM_TOLERANCE = 0.001f; // the table's fractions add to one within this
const char *LIMB_KEYS[RagdollParams::LIMBS] = { "head", "arm_l", "arm_r", "leg_l", "leg_r" };

float num(const Dictionary &d, const char *key, PackedStringArray &errors, const char *group) {
	if (d.has(key)) return float(double(d[key]));
	errors.push_back(String("ragdoll.") + group + ": falta '" + key + "'");
	return 0.f;
}

// [lo, hi] in degrees -> radians.
void range(const Dictionary &j, const char *key, float &lo, float &hi, PackedStringArray &errors) {
	const Array a = j.get(key, Array());
	if (a.size() < 2) {
		errors.push_back(String("ragdoll.joints: falta '") + key + "' de " + String(j.get("child", "?")));
		return;
	}
	lo = float(double(a[0])) * DEG_TO_RAD;
	hi = float(double(a[1])) * DEG_TO_RAD;
}
} // namespace

bool RagdollParams::load(const Dictionary &t, PackedStringArray &errors) {
	const int before = int(errors.size());
	const Dictionary s = t.get("solver", Dictionary());
	solver.substeps = int(num(s, "substeps", errors, "solver"));
	solver.joint_iterations = int(num(s, "joint_iterations", errors, "solver"));
	solver.friction = num(s, "friction", errors, "solver");
	solver.slope_span_m = num(s, "slope_span_m", errors, "solver");
	solver.rest_speed = num(s, "rest_speed_ms", errors, "solver");
	rest_after_s = num(s, "rest_after_s", errors, "solver");
	span_m = num(s, "span_m", errors, "solver");
	max_people = int(num(s, "max_people", errors, "solver"));
	rig.contact_points = int(num(s, "contact_points_per_body", errors, "solver"));
	rig.min_gyration_m = num(s, "min_gyration_m", errors, "solver");
	std::map<std::string, int> index;
	const Array bodies = t.get("bodies", Array());
	float mass_sum = 0;
	for (int i = 0; i < bodies.size(); i++) {
		const Dictionary b = bodies[i];
		phys::BodyDef def;
		for (const Variant &bone : Array(b.get("bones", Array()))) def.bones.push_back(int(double(bone)));
		def.mass = float(double(b.get("mass", 0.0)));
		mass_sum += def.mass;
		names.push_back(std::string(String(b.get("name", "")).utf8().get_data()));
		index[names.back()] = i;
		rig.bodies.push_back(def);
	}
	for (int i = 0; i < bodies.size(); i++) {
		const std::string parent = std::string(String(Dictionary(bodies[i]).get("parent", "")).utf8().get_data());
		if (parent.empty()) continue;
		if (!index.count(parent)) errors.push_back("ragdoll.bodies: pai desconhecido de " + String(names[size_t(i)].c_str()));
		else rig.bodies[size_t(i)].parent = index[parent];
	}
	if (rig.bodies.empty() || std::fabs(mass_sum - 1.f) > MASS_SUM_TOLERANCE) errors.push_back("ragdoll.bodies: as massas devem somar 1");
	std::map<std::string, Dictionary> by_child;
	const Array joints = t.get("joints", Array());
	for (int i = 0; i < joints.size(); i++) by_child[std::string(String(Dictionary(joints[i]).get("child", "")).utf8().get_data())] = joints[i];
	for (int i = 0; i < joints.size(); i++) {
		const Dictionary j = joints[i];
		const std::string child = std::string(String(j.get("child", "")).utf8().get_data());
		phys::JointDef d;
		if (!index.count(child) || rig.bodies[size_t(index[child])].parent < 0) {
			errors.push_back("ragdoll.joints: corpo sem pai: " + String(child.c_str()));
			continue;
		}
		d.child = index[child];
		d.parent = rig.bodies[size_t(d.child)].parent;
		const String neutral = j.get("neutral", "");
		d.neutral = neutral == "hanging" ? phys::Neutral::Hanging : neutral == "collinear" ? phys::Neutral::Collinear : phys::Neutral::Bind;
		d.ref_up = String(j.get("ref", "forward")) == "up";
		const std::string distal = std::string(String(j.get("distal", "")).utf8().get_data());
		if (distal != "farthest") {
			if (!index.count(distal)) errors.push_back("ragdoll.joints: distal desconhecido: " + String(distal.c_str()));
			else d.distal = index[distal];
		}
		// the right side is the mirror of the left: lateral and twist change sign and swap
		const std::string src = std::string(String(j.get("mirror_of", "")).utf8().get_data());
		const bool mirrored = !src.empty();
		Dictionary from = j;
		if (mirrored) {
			if (!by_child.count(src)) {
				errors.push_back("ragdoll.joints: mirror_of desconhecido: " + String(src.c_str()));
				continue;
			}
			from = by_child[src];
		}
		range(from, "flex", d.flex_lo, d.flex_hi, errors);
		range(from, "lateral", d.lat_lo, d.lat_hi, errors);
		range(from, "twist", d.twist_lo, d.twist_hi, errors);
		if (mirrored) {
			const float lo = d.lat_lo, tlo = d.twist_lo;
			d.lat_lo = -d.lat_hi;
			d.lat_hi = -lo;
			d.twist_lo = -d.twist_hi;
			d.twist_hi = -tlo;
		}
		rig.joints.push_back(d);
	}
	const Dictionary limbs = t.get("limbs", Dictionary());
	for (int k = 0; k < RagdollParams::LIMBS; k++) {
		const std::string name = std::string(String(limbs.get(LIMB_KEYS[k], "")).utf8().get_data());
		if (!index.count(name)) errors.push_back(String("ragdoll.limbs: corpo desconhecido em ") + LIMB_KEYS[k]);
		else limb_root[k] = index[name];
	}
	return int(errors.size()) == before;
}

} // namespace city
