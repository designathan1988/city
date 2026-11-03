// Building chunks: the far level built for every chunk with records; the near (+ parts) and mid levels built lazily,
// nearest first within a per-frame budget, when the camera comes within their reach, and freed beyond 1.5x it.
#include "render/buildings/building_renderer.h"

#include "render/buildings/civic.h"

#include "render/buildings/facade.h"

#include <algorithm>

using namespace godot;

namespace city {

namespace {
constexpr float AABB_PAD = 1.f, HALF = 0.5f, REACH_PAD = 0.25f, DROP_K = 1.25f;

void range(const MeshDraw &d, float begin, float end) {
	if (d.inst.is_valid())
		RenderingServer::get_singleton()->instance_geometry_set_visibility_range(d.inst, begin, end, 0, 0, RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
}

void commit(MeshDraw &d, const b40::Shell &s, const RID &scenario, const RID &material, bool shadows) {
	d.reset(scenario, material, shadows);
	s.commit_to(d.mesh);
	if (!s.empty()) {
		Vector3 mn, mx, pad(AABB_PAD, AABB_PAD, AABB_PAD);
		s.bounds(mn, mx);
		RenderingServer::get_singleton()->instance_set_custom_aabb(d.inst, AABB(mn - pad, mx - mn + pad * 2));
	}
	d.visible(!s.empty());
}

template <typename F>
void each_record(const CityState &s, int64_t key, F &&fn) {
	float x0 = float(key_x(key)) * CHUNK, z0 = float(key_y(key)) * CHUNK;
	s.buildings.query(x0, z0, x0 + CHUNK - b40::EPS, z0 + CHUNK - b40::EPS, [&](int id) {
		const Building &b = s.buildings.items[size_t(id)];
		if (b.floors > 0 || b40::is_open_lot(b.fn)) fn(b);
	});
}
} // namespace

bool BuildingRenderer::init(const RenderContext &ctx, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	material_ = ctx.material("building", errors);
	Dictionary t = ctx.table("buildings", errors);
	if (!errors.is_empty()) return false;
	return data_.load(t, errors);
}

void BuildingRenderer::reset() {
	for (auto &kv : chunks_) {
		Chunk &ch = kv.second;
		ch.far.free(); ch.near.free(); ch.parts.free(); ch.mid.free();
	}
	chunks_.clear();
	bodies_.clear();
	pending_ = 0;
}

void BuildingRenderer::ranges(Chunk &ch) const {
	range(ch.parts, 0, parts_end_);
	range(ch.near, 0, near_end_);
	range(ch.mid, near_end_, mid_end_);
	range(ch.far, mid_end_, far_end_);
}

float BuildingRenderer::reach(Level level) const { return (level == LEVEL_NEAR ? near_end_ : mid_end_) + CHUNK * REACH_PAD; }

void BuildingRenderer::set_ranges(const CityState &s) {
	state_ = &s;
	float detail = s.catalog.tune.render_building_detail_end_m;
	parts_end_ = detail * data_.parts_share;
	near_end_ = detail * data_.near_share;
	mid_end_ = s.catalog.tune.render_building_mid_end_m;
	far_end_ = s.catalog.tune.render_building_far_end_m;
}

void BuildingRenderer::drop_chunk(int64_t key) {
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	Chunk &ch = it->second;
	ch.far.free(); ch.near.free(); ch.parts.free(); ch.mid.free();
	chunks_.erase(it);
}

void BuildingRenderer::rebuild_chunk(const CityState &s, int64_t key) {
	set_ranges(s);
	b40::Shell far;
	far.tone = false;
	each_record(s, key, [&](const Building &rec) {
		b40::Placed pl;
		bodies_.place(data_, s, rec, pl);
		pl.lean = true;
		b40::emit_far(far, pl, true);
	});
	Chunk &ch = chunks_[key];
	ch.centre = Vector3((float(key_x(key)) + HALF) * CHUNK, 0, (float(key_y(key)) + HALF) * CHUNK);
	commit(ch.far, far, scenario_, material_, true);
	for (int l = 0; l < LEVELS; l++)
		if (ch.built[l]) build(ch, key, Level(l));
	ranges(ch);
}

void BuildingRenderer::build(Chunk &ch, int64_t key, Level level) {
	b40::Shell main, parts;
	main.coarse = level == LEVEL_MID;
	each_record(*state_, key, [&](const Building &rec) {
		b40::Placed pl;
		bodies_.place(data_, *state_, rec, pl);
		pl.coarse = level == LEVEL_MID;
		if (level == LEVEL_NEAR) b40::emit_facades(main, parts, pl);
		else b40::emit_far(main, pl, true);
		b40::emit_roofs(main, parts, pl);
		b40::emit_lot(main, parts, pl);
	});
	if (level == LEVEL_NEAR) {
		commit(ch.near, main, scenario_, material_, true);
		commit(ch.parts, parts, scenario_, material_, false);
	} else commit(ch.mid, main, scenario_, material_, true);
	ch.built[level] = true;
	ranges(ch);
}

void BuildingRenderer::drop(Chunk &ch, Level level) {
	if (level == LEVEL_NEAR) { ch.near.free(); ch.parts.free(); }
	else ch.mid.free();
	ch.built[level] = false;
}

void BuildingRenderer::update_view(const Vector3 &cam, BuildBudget &budget) {
	if (!state_) return;
	struct Want {
		float d;
		int64_t key;
		Level level;
	};
	std::vector<Want> want;
	for (auto &kv : chunks_) {
		Chunk &ch = kv.second;
		float dx = cam.x - ch.centre.x, dz = cam.z - ch.centre.z;
		float d = std::sqrt(dx * dx + dz * dz); // horizontal: the eye's height is not a distance to the chunk
		for (int l = 0; l < LEVELS; l++) {
			Level level = Level(l);
			if (!ch.built[l] && d < reach(level)) want.push_back({ d, kv.first, level });
			else if (ch.built[l] && d > reach(level) * DROP_K) drop(ch, level);
		}
	}
	std::sort(want.begin(), want.end(), [](const Want &a, const Want &b) { return a.d < b.d; });
	size_t n = 0;
	for (; n < want.size(); n++) {
		if (!budget.take()) break;
		build(chunks_[want[n].key], want[n].key, want[n].level);
	}
	pending_ = int(want.size() - n);
}

} // namespace city
