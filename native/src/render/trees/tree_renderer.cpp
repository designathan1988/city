// Streams the tree chunks (see tree_renderer.h). An instance carries its basis (turn, width, height and lean), its
// foot, and in its custom data the tint and the species index the shader reads its
// wind response by.
#include "render/trees/tree_renderer.h"

#include "core/math.h"
#include "render/owned_segments.h"
#include "render/roads/road_cut.h"

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

TreeRenderer::TreeRenderer() {
	tier_[TIER_NEAR].hand_one = [this](Built &b) { commit_near(b); };
	tier_[TIER_FAR].hand_one = [this](Built &b) { commit_far(b); };
}

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
	if (!kit_.build(t["trees"], body_material_, card_material_, errors) || !trees_.load(t["trees"], kit_, errors)) return false;
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
	for (Tier &t : tier_) t.clear();
	for (auto &kv : near_) {
		for (MultiMeshDraw &d : kv.second.body) d.free();
		for (MultiMeshDraw &d : kv.second.cast) d.free();
		for (MultiMeshDraw &d : kv.second.cards) d.free();
	}
	for (auto &kv : far_) kv.second.free();
	near_.clear();
	far_.clear();
	pending_ = 0;
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

void TreeRenderer::set_share(float tier) {
	const float share = trees_.tier_share(tier);
	if (trees_.share.load() == share) return;
	trees_.share.store(share);
	for (const auto &[key, ch] : near_) tier_[TIER_NEAR].revs.mark(key);
	for (const auto &[key, sp] : far_) tier_[TIER_FAR].revs.mark(key);
}

void TreeRenderer::mark_dirty(int64_t key) {
	tier_[TIER_NEAR].revs.mark(key);
	tier_[TIER_FAR].revs.mark(cell_key(int(std::floor(float(key_x(key)) / CHUNKS_PER_SUPER)), int(std::floor(float(key_y(key)) / CHUNKS_PER_SUPER))));
}

// What a chunk's trees, or a superchunk's cards, read of the city, copied under the map's shared lock: the roads, the lots, the
// land with its natural ground and its water, the paint, the planted trees and the clearings of the square, of the segments its
// chunks own (a street's trees stand along the whole segment) and of the lots that stand in it (a garden is planted over the
// whole lot), as far past them as the ground, the woods and the road pieces read; and the furniture of each owned segment, the
// one fact a box of records cannot give.
CityCut TreeRenderer::gather(const CityState &s, int64_t key, bool far) const {
	const int side = far ? CHUNKS_PER_SUPER : 1;
	const auto each_chunk = [&](auto &&fn) {
		for (int j = 0; j < side; j++)
			for (int i = 0; i < side; i++) fn(cell_key(key_x(key) * side + i, key_y(key) * side + j));
	};
	return cut_city(
			s, CUT_ROADS | CUT_BUILDINGS | CUT_NATURE | CUT_GROUND_BASE | CUT_WATER,
			[&](const CityState &live) {
				const float size = float(side) * CHUNK, x0 = float(key_x(key)) * size, z0 = float(key_y(key)) * size;
				CutBox box{ x0, z0, x0 + size, z0 + size };
				each_chunk([&](int64_t chunk) {
					each_owned_segment(live, chunk, [&](int sid) {
						const RoadSegment &g = live.roads.segs[size_t(sid)];
						box.take(g.minx, g.minz, g.maxx, g.maxz);
					});
					const float cx = float(key_x(chunk)) * CHUNK, cz = float(key_y(chunk)) * CHUNK;
					live.buildings.query(cx, cz, cx + CHUNK, cz + CHUNK, [&](int id) {
						const Building &b = live.buildings.items[size_t(id)];
						if (chunk_of(b.x, b.z) != chunk) return;
						const float r = std::hypot(b.w, b.d) * HALF; // a lot's corners stand within its half diagonal of its centre
						box.take(b.x - r, b.z - r, b.x + r, b.z + r);
					});
				});
				return box.grown(std::max({ ground_reach(live.catalog), tree_reach(trees_), road_reach(live.catalog, *look_) }));
			},
			[&](CityCut &cut, const CityState &live) {
				each_chunk([&](int64_t chunk) { each_owned_segment(live, chunk, [&](int sid) { keep_furniture(cut, live, sid); }); });
			});
}

std::shared_ptr<TreeRenderer::Built> TreeRenderer::build_near(const CityView &s, int64_t key) const {
	auto out = std::make_shared<Built>();
	std::vector<TreeInstance> trees;
	place_trees(s, *look_, trees_, kit_, key, trees);
	out->bufs.resize(size_t(kit_.count()));
	for (const TreeInstance &t : trees) {
		const TreeBasis b = tree_basis(t);
		mm_push(out->bufs[size_t(t.species)], b.x, b.y, b.z, t.p, Color(1, 1, 1, 1), t.tint.r, t.tint.g, t.tint.b, float(t.species));
	}
	for (int i = 0; i < kit_.count(); i++) { // commit_near sends a species' buffer to its body, its caster and its cards
		const bool casts = std::find(trees_.bed_species.begin(), trees_.bed_species.end(), i) == trees_.bed_species.end();
		const int64_t draws = 1 + (casts ? 1 : 0) + (kit_.species(i).cards.valid() ? 1 : 0);
		out->upload += out->bufs[size_t(i)].size() * int64_t(sizeof(float)) * draws;
	}
	return out;
}

std::shared_ptr<TreeRenderer::Built> TreeRenderer::build_far(const CityView &s, int64_t key) const {
	auto out = std::make_shared<Built>();
	out->far = true;
	for (int j = 0; j < CHUNKS_PER_SUPER; j++)
		for (int i = 0; i < CHUNKS_PER_SUPER; i++)
			place_far(s, *look_, trees_, kit_, cell_key(key_x(key) * CHUNKS_PER_SUPER + i, key_y(key) * CHUNKS_PER_SUPER + j), out->cards);
	out->centre = FAR_CARD_CENTRE;
	out->half_tall = kit_.pictures().frame_share() * HALF;
	out->card_material = card_material_;
	out->upload = far_cards_bytes(out->cards.size());
	return out;
}

void TreeRenderer::Built::bake() {
	far_mesh = bake_far_cards(cards, centre, half_tall, card_material);
	cards = std::vector<FarCard>();
	baked = true;
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
		ch.body[size_t(i)].set(scenario_, sp.body.rid, b.bufs[size_t(i)], box, false, LAYER_VEGETATION);
		ch.body[size_t(i)].range(end);
		if (casts) {
			ch.cast[size_t(i)].shadow_only = true;
			ch.cast[size_t(i)].set(scenario_, sp.caster.rid, b.bufs[size_t(i)], box, true, LAYER_VEGETATION);
			ch.cast[size_t(i)].range(end);
		}
		if (!sp.cards.valid()) continue;
		ch.cards[size_t(i)].set(scenario_, sp.cards.rid, b.bufs[size_t(i)], box, sp.cards_shadow, LAYER_VEGETATION);
		ch.cards[size_t(i)].range(end);
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
	swap_base(d.inst, d.mesh, b.far_mesh.release(), scenario_, trees_.far_shadows != 0, LAYER_VEGETATION);
	if (!d.inst.is_valid()) return;
	RenderingServer *rs = RenderingServer::get_singleton();
	rs->instance_set_custom_aabb(d.inst, box_of(b.key, SUPERCHUNK));
	rs->instance_geometry_set_visibility_range(d.inst, 0, trees_.far_m + SUPERCHUNK * DIAGONAL * HALF, 0, 0, RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
}

void TreeRenderer::submit(const CityState &s, int64_t key, float priority, bool far) {
	if (!s.jobs) return;
	Tier &t = tier_[far ? TIER_FAR : TIER_NEAR];
	t.set_pool(s.jobs.get());
	const uint64_t ticket = t.revs.ask(key);
	const JobKey job{ far ? JobLane::TREES_FAR : JobLane::TREES_NEAR, key };
	s.jobs->submit(job, priority, [this, &s, &t, key, job, ticket, priority, far]() {
		// The job copies what its trees read and places them from the copy, the map let go.
		const CityCut cut = gather(s, key, far);
		std::shared_ptr<Built> b = far ? build_far(cut, key) : build_near(cut, key);
		b->key = key;
		b->job = job;
		b->ticket = ticket;
		b->priority = priority;
		t.push(std::move(b));
	});
}

int TreeRenderer::pending() const {
	return pending_ + tier_[TIER_NEAR].pending() + tier_[TIER_FAR].pending();
}

void TreeRenderer::offer(const CityState &s, const Vector3 &cam, std::vector<FrameAdmission::Unit> &out) {
	pending_ = 0;
	stream(s, cam, false, out);
	stream(s, cam, true, out);
}

// One level: the marked cells drawn again, the missing ones round the camera asked, and the far let go. A marked cell whose
// result is on its way keeps its mark and is asked again when that one lands; a marked cell not drawn is built fresh when it comes.
void TreeRenderer::stream(const CityState &s, const Vector3 &cam, bool far, std::vector<FrameAdmission::Unit> &out) {
	Tier &t = tier_[far ? TIER_FAR : TIER_NEAR];
	float size = far ? SUPERCHUNK : CHUNK, reach = far ? trees_.far_m : trees_.near_m + trees_.fade_m;
	auto unit = [&](int64_t k, float d, int rank) {
		pending_++;
		out.push_back(FrameAdmission::Unit{ rank, d, 1, [this, &s, k, d, far]() {
					   pending_--;
					   submit(s, k, d, far);
				   } });
	};
	std::vector<int64_t> unmark;
	for (int64_t k : t.revs.wanted()) {
		if (t.revs.busy(k)) continue; // counted by the tier's pending
		if (!built(k, far)) {
			unmark.push_back(k);
			continue;
		}
		unit(k, distance_to(k, size, cam), 1);
	}
	for (int64_t k : unmark) t.revs.unwant(k);
	int r = int(std::ceil(reach / size)), cx = int(std::floor(cam.x / size)), cz = int(std::floor(cam.z / size));
	for (int j = cz - r; j <= cz + r; j++)
		for (int i = cx - r; i <= cx + r; i++) {
			int64_t k = cell_key(i, j);
			float d = distance_to(k, size, cam);
			if (d > reach + size * DIAGONAL * HALF || !s.terrain.in_bounds((i + HALF) * size, (j + HALF) * size, -size * HALF)) continue;
			if (!built(k, far) && !t.revs.busy(k)) unit(k, d, 0);
		}
	if (far) {
		for (auto it = far_.begin(); it != far_.end();) {
			if (distance_to(it->first, size, cam) <= reach + EVICT_CHUNKS * size) { ++it; continue; }
			t.drop(it->first);
			it->second.free();
			it = far_.erase(it);
		}
	} else {
		for (auto it = near_.begin(); it != near_.end();) {
			if (distance_to(it->first, size, cam) <= reach + EVICT_CHUNKS * size) { ++it; continue; }
			t.drop(it->first);
			for (MultiMeshDraw &d : it->second.body) d.free();
			for (MultiMeshDraw &d : it->second.cast) d.free();
			for (MultiMeshDraw &d : it->second.cards) d.free();
			it = near_.erase(it);
		}
	}
}

} // namespace city
