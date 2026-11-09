// The traffic agents' shapes of the shared parts and the heavy vehicles: the cabin, the parts, a wreck, the bicycle, the bus, what bus and truck share, the truck and the motorbike, the ragdoll (agents.json "shapes", read by agent_renderer.cpp).
#pragma once

#include "core/shape_fields.h"

namespace city {

#define AGENT_SHAPE_AGENT_CABIN(X, XI, XD, XV, XVI) \
	XI(rim_sides) XV(spoke_angles) X(spoke_w_m) X(spoke_t_m) X(boss_w_m) X(boss_l_m) X(seat_nose_m) \
	X(seat_nose_drop_m) X(seat_lip_m) X(seat_crease_m) X(seat_lap_m) X(back_top_round_m) X(back_top_in_m) \
	X(rest_depth_m) X(headrest_rod_m) X(headrest_rod_share) X(dash_lip_m) X(dash_foot_m) X(dash_waist)
struct CabinShape {
	SHAPE_DECLARE(AGENT_SHAPE_AGENT_CABIN)
};

#define AGENT_SHAPE_AGENT_PARTS(X, XI, XD, XV, XVI) \
	X(pod_squareness) X(pod_nose) X(hub_share) X(hub_proud_m) X(rim_share) X(spoke_shade) X(pan_gap_m) \
	X(tyre_section) X(tyre_lip) XI(shoulder_steps) XI(spokes) XI(boss_sides) X(rim_face) X(spoke_root) \
	X(spoke_tip) X(spoke_face) X(lip_in) X(rotor_r) X(rotor_shade) X(rotor_deep) X(boss_r) X(boss_face) X(wire_m) \
	X(arch_pad_m)
struct PartsShape {
	SHAPE_DECLARE(AGENT_SHAPE_AGENT_PARTS)
};

#define AGENT_SHAPE_AGENT_WRECK(X, XI, XD, XV, XVI) \
	X(dent_wide_m) X(dent_fine_m) X(dent_wide_deep_m) X(dent_fine_deep_m) X(sag_m) X(dent_x) X(dent_z) X(z_pace) \
	X(y_pace)
struct WreckShape {
	SHAPE_DECLARE(AGENT_SHAPE_AGENT_WRECK)
};

#define AGENT_SHAPE_BICYCLE_MESH(X, XI, XD, XV, XVI) \
	X(stay_x) X(fork_x) X(saddle_long) X(saddle_half_w) X(saddle_top) X(bar_half) X(thin)
struct BicycleShape {
	SHAPE_DECLARE(AGENT_SHAPE_BICYCLE_MESH)
};

#define AGENT_SHAPE_BUS_BODY(X, XI, XD, XV, XVI) \
	X(partition_thick_m) X(well_gap_m)
struct BusShape {
	SHAPE_DECLARE(AGENT_SHAPE_BUS_BODY)
};

#define AGENT_SHAPE_HEAVY(X, XI, XD, XV, XVI) \
	X(proud_m) X(glass_inset_m) X(pillar_m) X(door_split_m)
struct HeavyShape {
	SHAPE_DECLARE(AGENT_SHAPE_HEAVY)
};

#define AGENT_SHAPE_HEAVY_MESHES(X, XI, XD, XV, XVI) \
	X(mirror_w) X(mirror_h) X(mirror_d) X(cab_door_seam) X(tail_lens_h_m) X(moto_lamp_d)
struct HeavyMeshesShape {
	SHAPE_DECLARE(AGENT_SHAPE_HEAVY_MESHES)
};

#define AGENT_SHAPE_RAGDOLL(X, XI, XD, XV, XVI) \
	X(air_keep) X(limb_half_m) X(slide_keep) XI(iterations) X(top_over_head) X(belly_out_m) X(knee_least) \
	X(elbow_least) X(fold_least) X(resting_ms) X(resting_s) X(step_s) XI(steps_most) X(writhe_rate) X(writhe_phase) \
	X(writhe_rise_s) X(writhe_fade_s) X(writhe_floor) X(writhe_hip_share) X(writhe_spine_share) X(writhe_pull)
struct RagdollShape {
	SHAPE_DECLARE(AGENT_SHAPE_RAGDOLL)
};

} // namespace city
