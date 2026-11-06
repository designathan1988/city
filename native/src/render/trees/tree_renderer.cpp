// Streams the tree chunks (see tree_renderer.h). An instance carries its basis (turn, width, height and lean), its
// foot, and in its custom data the tint and the species index the shader reads its
// wind response by.
#include "render/trees/tree_renderer.h"
#include "render/chunk_jobs.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f, EVICT_CHUNKS = 2.f;
constexpr int TIER_NEAR = 0, TIER_FAR = 1;
constexpr int CHUNKS_PER_SUPER = int(SUPERCHUNK / CHUNK); // per axis
const float DIAGONAL = std::sqrt(2.f);

AABB box_of(int64_t key, float size) { return AABB(Vector3(key_x(key) * size, AABB_MIN_Y, key_y(key) * size), Vector3(size, AABB_H, size)); }

float distance_to(int64_t key, float size, const Vector3 &cam) {
	float dx = (key_x(key) + HALF) * size - cam.x, dz = (key_y(key) + HALF) * size - cam.z;
	return std::sqrt(dx * dx + dz * dz);
}
} // namespace

bool TreeRenderer::init(const RenderContext &ctx, const StreetLook &look, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	body_material_ = ctx.material("vegetation", errors);
	card_material_ = ctx.material("vegetation_cards", errors);
	look_ = &look;
	Dictionary t = ctx.table("streets", errors);
	if (!errors.is_empty()) return false;
	if (!t.has("trees")) {
		errors.push_back("streets.json: falta trees");
		return false;
	}
	if (!kit_.build(t["trees"], errors) || !trees_.load(t["trees"], kit_, errors)) return false;
	set_params(body_material_);
	set_params(card_material_);
	return true;
}

// The shader's share of the data: where near meets far and the leaf atlas.
void TreeRenderer::set_params(const RID &material) const {
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->material_set_param(material, "near_m", trees_.near_m);
	rs->material_set_param(material, "fade_m", trees_.fade_m);
	rs->material_set_param(material, "leaf_atlas", kit_.atlas().texture());
	rs->material_set_param(material, "tree_albedo", kit_.pictures().albedo());
	rs->material_set_param(material, "tree_normal", kit_.pictures().normal());
	rs->material_set_param(material, "picture_frames", kit_.pictures().frames());
	rs->material_set_param(material, "picture_rows", kit_.pictures().rows());
	rs->material_set_param(material, "picture_top_deg", kit_.pictures().top_elevation_deg());
}

void TreeRenderer::reset() {
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		for (Tier &t : tier_) {
			t.done.clear();
			t.requested.clear();
			t.dirty.clear();
		}
	}
	for (auto &kv : near_) {
		for (MultiMeshDraw &d : kv.second.body) d.free();
		for (MultiMeshDraw &d : kv.second.cast) d.free();
		for (MultiMeshDraw &d : kv.second.cards) d.free();
	}
	for (auto &kv : far_) kv.second.free();
	near_.clear();
	far_.clear();
	pending_ = 0;
	gardens_.clear();
}

int TreeRenderer::near_trees() const {
	int n = 0;
	for (const auto &[key, ch] : near_) n += ch.trees;
	return n;
}

int TreeRenderer::near_plants() const {
	int n = 0;
	for (const auto &[key, ch] : near_) n += ch.plants;
	return n;
}

void TreeRenderer::set_share(float share) {
	if (trees_.share.load() == share) return;
	trees_.share.store(share);
	for (const auto &[key, ch] : near_) tier_[TIER_NEAR].dirty.insert(key);
	for (const auto &[key, sp] : far_) tier_[TIER_FAR].dirty.insert(key);
}

void TreeRenderer::mark_dirty(int64_t key) {
	tier_[TIER_NEAR].dirty.insert(key);
	tier_[TIER_FAR].dirty.insert(cell_key(int(std::floor(float(key_x(key)) / CHUNKS_PER_SUPER)), int(std::floor(float(key_y(key)) / CHUNKS_PER_SUPER))));
}

TreeRenderer::Built TreeRenderer::build_near(const CityState &s, int64_t key) const {
	Built out;
	out.key = key;
	std::vector<TreeInstance> trees;
	place_trees(s, *look_, trees_, kit_, gardens_, key, trees);
	out.bufs.resize(size_t(kit_.count()));
	for (const TreeInstance &t : trees) {
		const float c = std::cos(t.yaw), sn = std::sin(t.yaw), deep = t.depth > 0 ? t.depth : t.width;
		mm_push(out.bufs[size_t(t.species)], Vector3(c, 0, -sn) * t.width, Vector3(t.lean_x, 1, t.lean_z) * t.height, Vector3(sn, 0, c) * deep, t.p, Color(1, 1, 1, 1),
				t.tint.r, t.tint.g, t.tint.b, float(t.species));
	}
	return out;
}

TreeRenderer::Built TreeRenderer::build_far(const CityState &s, int64_t key) const {
	Built out;
	out.key = key;
	std::vector<FarCard> cards;
	for (int j = 0; j < CHUNKS_PER_SUPER; j++)
		for (int i = 0; i < CHUNKS_PER_SUPER; i++)
			place_far(s, *look_, trees_, kit_, gardens_, cell_key(key_x(key) * CHUNKS_PER_SUPER + i, key_y(key) * CHUNKS_PER_SUPER + j), cards);
	out.far = bake_far_cards(cards, FAR_CARD_CENTRE, kit_.pictures().frame_share() * HALF);
	return out;
}

void TreeRenderer::commit_near(Built &b) {
	Near &ch = near_[b.key];
	int n = kit_.count();
	ch.body.resize(size_t(n));
	ch.cast.resize(size_t(n));
	ch.cards.resize(size_t(n));
	ch.trees = 0;
	for (const PackedFloat32Array &buf : b.bufs) ch.trees += int(buf.size() / MM_STRIDE);
	ch.plants = int(b.bufs[size_t(trees_.shrub_species)].size() + b.bufs[size_t(trees_.hedge_species)].size()) / MM_STRIDE;
	for (int species : trees_.flowering_species) ch.plants += int(b.bufs[size_t(species)].size() / MM_STRIDE);
	for (int species : trees_.bed_species) ch.plants += int(b.bufs[size_t(species)].size() / MM_STRIDE);
	AABB box = box_of(b.key, CHUNK);
	float end = trees_.near_m + trees_.fade_m + CHUNK * DIAGONAL * HALF;
	for (int i = 0; i < n; i++) {
		const TreeSpecies &sp = kit_.species(i);
		// A flower bed casts nothing: its shadow would not read, and the town's beds cost as much as its buildings'.
		const bool casts = std::find(trees_.bed_species.begin(), trees_.bed_species.end(), i) == trees_.bed_species.end();
		ch.body[size_t(i)].set(scenario_, sp.body->get_rid(), body_material_, b.bufs[size_t(i)], box, false, LAYER_VEGETATION);
		ch.body[size_t(i)].range(0, end);
		if (casts) {
			ch.cast[size_t(i)].shadow_only = true;
			ch.cast[size_t(i)].set(scenario_, sp.caster->get_rid(), body_material_, b.bufs[size_t(i)], box, true, LAYER_VEGETATION);
			ch.cast[size_t(i)].range(0, end);
		}
		if (sp.cards.is_null()) continue;
		ch.cards[size_t(i)].set(scenario_, sp.cards->get_rid(), card_material_, b.bufs[size_t(i)], box, sp.cards_cast, LAYER_VEGETATION);
		ch.cards[size_t(i)].range(0, end);
	}
}

void TreeRenderer::FarDraw::free() {
	RenderingServer *rs = RenderingServer::get_singleton();
	if (inst.is_valid()) rs->free_rid(inst);
	if (mesh.is_valid()) rs->free_rid(mesh);
	inst = mesh = RID();
}

void TreeRenderer::commit_far(Built &b) {
	FarDraw &d = far_[b.key];
	swap_base(d.inst, d.mesh, b.far.release(), scenario_, card_material_, trees_.far_shadows != 0, LAYER_VEGETATION);
	if (!d.inst.is_valid()) return;
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->instance_set_custom_aabb(d.inst, box_of(b.key, SUPERCHUNK));
	rs->instance_geometry_set_visibility_range(d.inst, 0, trees_.far_m + SUPERCHUNK * DIAGONAL * HALF, 0, 0, RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
}

void TreeRenderer::submit(const CityState &s, int64_t key, float priority, bool far) {
	if (!s.jobs) return;
	Tier &t = tier_[far ? TIER_FAR : TIER_NEAR];
	uint64_t seq;
	{
		std::lock_guard<std::mutex> lk(queue_mutex_);
		seq = ++seq_;
		t.requested[key] = seq;
	}
	s.jobs->submit(JobKey{ far ? JobLane::TREES_FAR : JobLane::TREES_NEAR, key }, priority, [this, &s, &t, key, seq, priority, far]() {
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		Built b = far ? build_far(s, key) : build_near(s, key);
		b.seq = seq;
		b.priority = priority;
		std::lock_guard<std::mutex> lk(queue_mutex_);
		t.done.push_back(std::move(b));
	});
}

void TreeRenderer::commit(const std::chrono::steady_clock::time_point &until) {
	commit_done(queue_mutex_, tier_[TIER_NEAR].done, tier_[TIER_NEAR].requested, until, [this](Built &b) { commit_near(b); });
	commit_done(queue_mutex_, tier_[TIER_FAR].done, tier_[TIER_FAR].requested, until, [this](Built &b) { commit_far(b); });
}

int TreeRenderer::pending() const {
	std::lock_guard<std::mutex> lk(queue_mutex_);
	return pending_ + int(tier_[TIER_NEAR].dirty.size() + tier_[TIER_FAR].dirty.size()) + int(tier_[TIER_NEAR].requested.size() + tier_[TIER_NEAR].done.size() + tier_[TIER_FAR].requested.size() + tier_[TIER_FAR].done.size());
}

void TreeRenderer::sync(const CityState &s, const Vector3 &cam, BuildBudget &budget) {
	pending_ = 0;
	stream(s, cam, budget, false);
	stream(s, cam, budget, true);
}

// One level: rebuild the dirty cells, ask the missing ones round the camera, nearest first, and let go of the far.
void TreeRenderer::stream(const CityState &s, const Vector3 &cam, BuildBudget &budget, bool far) {
	Tier &t = tier_[far ? TIER_FAR : TIER_NEAR];
	float size = far ? SUPERCHUNK : CHUNK, reach = far ? trees_.far_m : trees_.near_m + trees_.fade_m;
	auto busy = [&](int64_t k) {
		std::lock_guard<std::mutex> lk(queue_mutex_);
		return t.requested.count(k) > 0;
	};
	auto built = [&](int64_t k) { return far ? far_.count(k) > 0 : near_.count(k) > 0; };
	for (auto it = t.dirty.begin(); it != t.dirty.end();) {
		if (!built(*it) || busy(*it)) { it = t.dirty.erase(it); continue; } // a chunk not drawn is built fresh when it comes
		if (!budget.take()) break;
		submit(s, *it, distance_to(*it, size, cam), far);
		it = t.dirty.erase(it);
	}
	int r = int(std::ceil(reach / size)), cx = int(std::floor(cam.x / size)), cz = int(std::floor(cam.z / size));
	std::vector<std::pair<float, int64_t>> want;
	for (int j = cz - r; j <= cz + r; j++)
		for (int i = cx - r; i <= cx + r; i++) {
			int64_t k = cell_key(i, j);
			float d = distance_to(k, size, cam);
			if (d > reach + size * DIAGONAL * HALF || !s.terrain.in_bounds((i + HALF) * size, (j + HALF) * size, -size * HALF)) continue;
			if (!built(k) && !busy(k)) want.push_back({ d, k });
		}
	auto evict = [&](int64_t k) {
		std::lock_guard<std::mutex> lk(queue_mutex_);
		t.requested.erase(k);
	};
	if (far) {
		for (auto it = far_.begin(); it != far_.end();) {
			if (distance_to(it->first, size, cam) <= reach + EVICT_CHUNKS * size) { ++it; continue; }
			evict(it->first);
			it->second.free();
			it = far_.erase(it);
		}
	} else {
		for (auto it = near_.begin(); it != near_.end();) {
			if (distance_to(it->first, size, cam) <= reach + EVICT_CHUNKS * size) { ++it; continue; }
			evict(it->first);
			for (MultiMeshDraw &d : it->second.body) d.free();
			for (MultiMeshDraw &d : it->second.cast) d.free();
			for (MultiMeshDraw &d : it->second.cards) d.free();
			it = near_.erase(it);
		}
	}
	std::sort(want.begin(), want.end());
	int n = 0;
	for (const auto &w : want) {
		if (!budget.take()) break;
		submit(s, w.second, w.first, far);
		++n;
	}
	pending_ += int(want.size()) - n;
}

} // namespace city
