// The street lamps' pools of light, their one owner (the reference game lays one under each lamp): each lit lamp the placement stands (sim/roads/street_furniture.h: a row's pole lamp or column, a junction's corner pole, a lone column,
// a pole the player put up) casts one pool from where it stands, onto whatever lies under it (road, junction, kerb, footway, a lot's
// front) and nowhere else; past the lamps with a light of their own (render/props/lamp_lights.h, `lamp_lit`) the pools light the city.
// A lighting column's pool reaches across the street, a pole lamp's is round under its head (sim lamp_pool).
#pragma once

#include "core/job_pool.h"
#include "render/mesh_queue.h"
#include "render/props/street_props.h"
#include "render/render_context.h"
#include "render/roads/road_look.h"
#include "render/rs_util.h"
#include "sim/state/city_cut.h"
#include "sim/state/city_view.h"

#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <unordered_map>
#include <vector>

namespace city {

// A pool on the ground: its middle on the street's top under the light, its half-axes across the street (toward the road), up through the
// slab it lights and along the street at its grade, and the lens it falls from (the point the props hand the lights).
struct LampPoolPlace {
	godot::Vector3 centre, across, up, along, lens;
};

// The pools of the lit lamps of the square [x0, x0 + size) x [z0, z0 + size) of whole chunks: of the segments those chunks own and of the
// player's poles standing in them, appended to `out`.
void place_lamp_pools(const CityView &s, const StreetLook &L, const FurnitureLook &F, float x0, float z0, float size, std::vector<LampPoolPlace> &out);

// What a superchunk's job lays: a MultiMesh row a pool (rs_util.h mm_push_plain: a unit box whose basis is the pool's whole axes),
// and the box they all stand in.
struct PoolRows {
	godot::PackedFloat32Array rows;
	godot::AABB bounds;
};

// The pools drawn: a MultiMesh of them a superchunk (core/grid.h SUPERCHUNK), streamed by RenderWorld as far as the far roads (no street
// goes dark before the buildings end) and let go past it, made again when the roads, the street works or the lots of one of its chunks
// change; its job copies the superchunk's roads and street works with their lamps placed anew (city_cut.h keep_lamps) and lays them with
// the map let go. Drawn only at night (lay_by_night): by day no instance is.
class LampPools : public MeshQueueOf<PoolRows> {
public:
	bool init(const RenderContext &ctx, const StreetLook &look, const FurnitureLook &furniture, godot::PackedStringArray &errors);
	void reset();
	void submit(const CityState &s, int64_t super, JobPool &pool, float priority);
	void drop_super(int64_t super);
	void lay_by_night(float night); // the sky's night share (0 by day): the pools are drawn while it is above 0
	// Tests: the middles of the pools drawn within `r` of (x, z) on the ground, read back from the GPU (multimesh_get_buffer).
	void drawn_near(float x, float z, float r, godot::PackedVector3Array &out) const;

protected:
	void commit_one(Built &b) override;

private:
	CityCut gather(const CityState &s, int64_t super) const;
	godot::RID scenario_;
	const StreetLook *look_ = nullptr;
	const FurnitureLook *furniture_ = nullptr;
	OwnedMesh box_;
	std::unordered_map<int64_t, MultiMeshDraw> supers_;
	bool lit_ = false; // night: the pools are drawn
};

class PropRenderer;
// Tests: the pools round (x, z) within `r`: `drawn`, their middles as the GPU holds them; `wanted`, one a lit lamp of the chunks the circle
// meets as the placement stands them (the live city, the map taken shared), its lens in `heads` at the same index; `lenses`, the lit
// lamps' lenses the props handed the lights (render/props/lamp_lights.h).
godot::Dictionary lamp_pool_census(const CityState &s, const StreetLook &L, const LampPools &pools, const PropRenderer &props, float x, float z, float r);

} // namespace city
