// Road chunk meshes: every segment (by its midpoint) and node of a 256 m chunk meshed into one surface drawn with the
// "road" material; the pieces are in road_segment.cpp, road_junction.cpp and road_markings.cpp. The material's night
// pools repeat every lamp step of the simulation's street furniture (its only owner).
#include "render/roads/road_renderer.h"

#include "render/roads/road_mesh.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>

using namespace godot;

namespace city {

bool RoadRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("road", errors);
	surface_material_ = material_; // every chunk's mesh is made with it (surface_data.h)
	Dictionary t = ctx.table("streets", errors);
	if (!errors.is_empty()) return false;
	if (!ctx.catalog) {
		errors.push_back("render sem catálogo");
		return false;
	}
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material_, "pool_step", ctx.catalog->furniture.lamps.span_m);
	if (!look_.load(t, errors)) return false;
	look_.ramp_full = ctx.catalog->furniture.crossing.zebra_from_m; // a dropped kerb is the zebra's: its ramp lands where the zebra does
	look_.ramp_flat = ctx.catalog->furniture.crossing.zebra_to_m;
	look_.h_sidewalk = ctx.catalog->furniture.sidewalk.height_m; // the footway's top, where the walkers stand too
	return true;
}

void RoadRenderer::build_cpu(const CityState &s, int64_t key, MeshBuilder &mb) const {
	emit_chunk(RoadEmit{ s, look_, mb, shapes_ }, key);
}

// A chunk draws every segment whose middle it holds, whole: its box is the chunk's column grown to what the job emitted
// (asked only for a chunk that emitted something).
AABB chunk_bounds(int64_t key, float size, const AABB &emitted) {
	const AABB column(Vector3(float(key_x(key)) * size, AABB_MIN_Y, float(key_y(key)) * size), Vector3(size, AABB_H, size));
	return column.merge(emitted);
}

void RoadRenderer::submit(const CityState &s, int64_t key, JobPool &pool, float priority) {
	submit_job(s, pool, JobKey{ JobLane::ROADS, key }, key, priority, [this, &s, key](MeshBuilder &mb) { build_cpu(s, key, mb); });
}

} // namespace city
