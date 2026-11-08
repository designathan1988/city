// Places and streams the ground cover (see cover_renderer.h).
#include "render/cover/cover_renderer.h"

#include "core/math.h"
#include "core/random.h"
#include "sim/terrain/terrain_gen.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
// a chunk's box from under the deepest ground to over the highest (it is culled by it, never drawn); a chunk is dropped two chunks
// past its reach, so it is not dropped and loaded again at one border; the cover's stream of a cell's hash
constexpr float COVER_MIN_Y = -100.f, COVER_SPAN_Y = 700.f, HALF = 0.5f, DROP_CHUNKS = 2.f;
constexpr uint32_t COVER_SALT = 0xc07e12u;
// Hash streams of one lattice cell.
enum Stream : uint32_t { KEEP = 0, POS_X, POS_Z, SIZE, YAW, TINT, VARIANT };

float h01(uint32_t h, Stream s) { return hash_float(h + s); }

float distance_to(int64_t key, const Vector3 &cam) {
	float dx = (key_x(key) + HALF) * CHUNK - cam.x, dz = (key_y(key) + HALF) * CHUNK - cam.z;
	return std::sqrt(dx * dx + dz * dz);
}

// How much of a kind grows at a point: what the player painted; the grass stands up among the painted scrub and flowers;
// cactus is the biome's own.
float density(const CityState &s, const CoverKit &kit, CoverPiece p, const EcologyField &eco, float x, float z) {
	const GroundPaint &g = s.nature.ground;
	switch (p) {
		case CoverPiece::ROCKS: return g.cover_at(CoverKind::ROCKS, x, z);
		case CoverPiece::SCRUB: return g.cover_at(CoverKind::SCRUB, x, z);
		case CoverPiece::CACTUS: return eco.near(x, z).cactus;
		case CoverPiece::TUFTS: return std::max(g.cover_at(CoverKind::SCRUB, x, z), g.cover_at(CoverKind::FLOWERS, x, z)) * kit.tuft_share;
		default: return g.cover_at(CoverKind::FLOWERS, x, z);
	}
}

bool any_cactus(const EcologyField &eco) {
	return std::any_of(eco.at.begin(), eco.at.end(), [](const Community &c) { return c.cactus > 0; });
}
} // namespace

CoverRenderer::CoverRenderer() {
	queue_.hand_one = [this](Built &b) { commit_one(b); };
}

bool CoverRenderer::init(const RenderContext &ctx, GroundKept &ground, PackedStringArray &errors) {
	scenario_ = ctx.scenario;
	grounds_ = &ground;
	plant_material_ = ctx.material("cover_plant", errors);
	rock_material_ = ctx.material("cover_rock", errors);
	shore_material_ = ctx.material("cover_shore", errors);
	const Dictionary table = ctx.table("cover", errors);
	if (!errors.is_empty()) return false;
	if (!ctx.catalog) {
		errors.push_back("cover: the render has no catalogue");
		return false;
	}
	const DataValue t = DataReader{ errors, "cover.json" }.at(table); // render/data_reader.h
	const int laid = ctx.catalog->nature.elements.first_effect;
	if (!kit_.build(t, errors) || !models_.load(t["elements"], laid, errors) || !shore_.load(t["shore"], errors)) return false;
	draws_ = 0;
	for (int i = 0; i < COVER_PIECES; i++) {
		first_[size_t(i)] = draws_;
		draws_ += int(kit_.look(CoverPiece(i)).variants.size());
	}
	RenderingServer *rs = RenderingServer::get_singleton();
	for (const RID &m : { plant_material_, rock_material_ }) {
		rs->material_set_param(m, "near_m", kit_.reach_m);
		rs->material_set_param(m, "fade_m", kit_.fade_m);
	}
	rs->material_set_param(shore_material_, "near_m", shore_.reach_m);
	rs->material_set_param(shore_material_, "fade_m", kit_.fade_m);
	return true;
}

void CoverRenderer::reset() {
	queue_.clear();
	wet_.clear();
	for (auto &kv : chunks_)
		for (MultiMeshDraw &d : kv.second) d.free();
	chunks_.clear();
	pending_ = 0;
}

void CoverRenderer::place(const CityState &s, int64_t key, const ChunkGround &l, Built &out) const {
	if (l.eco.side == 0) return;
	const bool painted = s.nature.ground.any_cover(), cactus = any_cactus(l.eco);
	for (int p = 0; p < COVER_PIECES; p++) {
		if (CoverPiece(p) == CoverPiece::CACTUS ? !cactus : !painted) continue;
		const CoverLook &k = kit_.look(CoverPiece(p));
		int n = int(CHUNK / k.spacing_m), cx = key_x(key), cz = key_y(key);
		for (int j = 0; j < n; j++)
			for (int i = 0; i < n; i++) {
				int ix = cx * n + i, iz = cz * n + j;
				uint32_t h = hash_combine(hash_combine(hash_combine(uint32_t(ix), uint32_t(iz)), COVER_SALT ^ s.seed), uint32_t(p));
				float x = (float(ix) + HALF + (h01(h, POS_X) - HALF)) * k.spacing_m, z = (float(iz) + HALF + (h01(h, POS_Z) - HALF)) * k.spacing_m;
				float d = density(s, kit_, CoverPiece(p), l.eco, x, z);
				if (d < k.least || h01(h, KEEP) > std::sqrt(d)) continue;
				if (!s.terrain.in_bounds(x, z, k.spacing_m) || s.terrain.is_water(x, z, 0.f)) continue;
				if (near_road(s, l, x, z, k.road_margin_m) || in_lot(s, l, k.road_margin_m, x, z)) continue;
				float size = k.size0 + (k.size1 - k.size0) * h01(h, SIZE), yaw = TAU_F * h01(h, YAW);
				float c = std::cos(yaw) * size, sn = std::sin(yaw) * size, tint = 1.f + (h01(h, TINT) * 2.f - 1.f) * k.tint_vary;
				float height = size * k.tall, y = s.terrain.height_at(x, z) - height * k.sink;
				int v = std::min(int(k.variants.size()) - 1, int(h01(h, VARIANT) * float(k.variants.size())));
				mm_push(out.bufs[size_t(first_[size_t(p)] + v)], Vector3(c, 0, -sn), Vector3(0, height, 0), Vector3(sn, 0, c), Vector3(x, y, z), Color(1, 1, 1, 1), tint, tint, tint, 0.f);
			}
	}
}

void CoverRenderer::place_elements(const CityState &s, int64_t key, Built &out) const {
	const std::vector<ElementItem> *items = s.nature.elements.in_chunk(key);
	if (!items) return;
	for (const ElementItem &e : *items) {
		if (e.kind >= models_.kinds()) continue; // an effect: render/element_effects.gd
		int n = models_.variants(e.kind), at = models_.first(e.kind) + std::min(n - 1, int(e.seed * float(n)));
		const ElementModel &m = models_.all()[size_t(at)];
		float k = e.size * m.fit, c = std::cos(e.yaw) * k, sn = std::sin(e.yaw) * k;
		Vector3 foot(e.x, s.terrain.height_at(e.x, e.z) - m.foot * k, e.z);
		mm_push(out.bufs[size_t(draws_ + at)], Vector3(c, 0, -sn), Vector3(0, k, 0), Vector3(sn, 0, c), foot, Color(1, 1, 1, 1), 1, 1, 1, 0);
	}
}

// A river's stones, on the same lattice rule as the cover and with its rocks' models, drawn apart: they reach further.
void CoverRenderer::place_shore(const CityState &s, int64_t key, const ChunkGround &l, Built &out) const {
	const CoverLook &k = kit_.look(CoverPiece::ROCKS);
	const int n = int(CHUNK / shore_.spacing_m), cx = key_x(key), cz = key_y(key), first = draws_ + int(models_.all().size());
	const float step = CHUNK / float(n);
	for (int j = 0; j < n; j++)
		for (int i = 0; i < n; i++) {
			int ix = cx * n + i, iz = cz * n + j;
			uint32_t h = hash_combine(hash_combine(hash_combine(uint32_t(ix), uint32_t(iz)), COVER_SALT ^ s.seed), uint32_t(COVER_PIECES));
			float x = (float(ix) + h01(h, POS_X)) * step, z = (float(iz) + h01(h, POS_Z)) * step;
			if (!s.terrain.in_bounds(x, z, step)) continue;
			float level = shore_level(s.terrain, x, z, shore_.search_m);
			if (level <= TerrainGen::NO_WATER) continue;
			float ground = s.terrain.height_at(x, z);
			if (h01(h, KEEP) > shore_.odds(ground - level) || near_road(s, l, x, z, k.road_margin_m) || in_lot(s, l, k.road_margin_m, x, z)) continue;
			float size = shore_.size0 + (shore_.size1 - shore_.size0) * std::pow(h01(h, SIZE), shore_.size_power), yaw = TAU_F * h01(h, YAW);
			float c = std::cos(yaw) * size, sn = std::sin(yaw) * size, tint = 1.f + (h01(h, TINT) * 2.f - 1.f) * k.tint_vary;
			float height = size * k.tall, y = ground - height * k.sink;
			int v = std::min(int(k.variants.size()) - 1, int(h01(h, VARIANT) * float(k.variants.size())));
			mm_push(out.bufs[size_t(first + v)], Vector3(c, 0, -sn), Vector3(0, height, 0), Vector3(sn, 0, c), Vector3(x, y, z), Color(1, 1, 1, 1), tint, tint, tint, 0.f);
		}
}

// The chunk's roads, lots and ecosystem gathered once for both lattices (the shore's stones read no ecosystem).
std::shared_ptr<CoverRenderer::Built> CoverRenderer::build(const CityState &s, int64_t key) const {
	auto out = std::make_shared<Built>();
	out->bufs.resize(size_t(draws_) + models_.all().size() + kit_.look(CoverPiece::ROCKS).variants.size());
	const bool plants = s.nature.nature.region >= 0 || s.nature.ground.any_cover();
	const bool shore = shore_near(s.terrain, key_x(key) * CHUNK, key_y(key) * CHUNK, CHUNK, shore_.search_m);
	const std::shared_ptr<const ChunkGround> l = plants || shore ? grounds_->of(s, key) : nullptr;
	if (plants) place(s, key, *l, *out);
	place_elements(s, key, *out);
	if (shore) place_shore(s, key, *l, *out);
	return out;
}

void CoverRenderer::commit_one(Built &b) {
	std::vector<MultiMeshDraw> &ch = chunks_[b.key];
	const CoverLook &rocks = kit_.look(CoverPiece::ROCKS);
	const size_t stones = size_t(draws_) + models_.all().size();
	ch.resize(stones + rocks.variants.size());
	AABB box(Vector3(key_x(b.key) * CHUNK, COVER_MIN_Y, key_y(b.key) * CHUNK), Vector3(CHUNK, COVER_SPAN_Y, CHUNK));
	float end = kit_.reach_m + kit_.fade_m + CHUNK * SQRT2_F * HALF;
	for (int p = 0; p < COVER_PIECES; p++) {
		const CoverLook &k = kit_.look(CoverPiece(p));
		for (size_t v = 0; v < k.variants.size(); v++) {
			size_t at = size_t(first_[size_t(p)]) + v;
			ch[at].set(scenario_, k.variants[v]->get_rid(), k.sways ? plant_material_ : rock_material_, b.bufs[at], box, k.casts, LAYER_VEGETATION);
			ch[at].range(0, end);
		}
	}
	for (size_t i = 0; i < models_.all().size(); i++) {
		size_t at = size_t(draws_) + i;
		ch[at].set(scenario_, models_.all()[i].mesh->get_rid(), RID(), b.bufs[at], box, false, LAYER_VEGETATION);
		ch[at].range(0, end);
	}
	for (size_t v = 0; v < rocks.variants.size(); v++) {
		ch[stones + v].set(scenario_, rocks.variants[v]->get_rid(), shore_material_, b.bufs[stones + v], box, rocks.casts, LAYER_VEGETATION);
		ch[stones + v].range(0, shore_.reach_m + kit_.fade_m + CHUNK * SQRT2_F * HALF);
	}
}

void CoverRenderer::submit(const CityState &s, int64_t key, float priority) {
	if (!s.jobs) return;
	queue_.set_pool(s.jobs.get());
	const uint64_t ticket = queue_.revs.ask(key);
	const JobKey job{ JobLane::COVER, key };
	s.jobs->submit(job, priority, [this, &s, key, job, ticket, priority]() {
		std::shared_ptr<Built> b;
		{
			std::shared_lock<std::shared_mutex> map(s.map_mutex);
			b = build(s, key);
		}
		b->key = key;
		b->job = job;
		b->ticket = ticket;
		b->priority = priority;
		queue_.push(std::move(b));
	});
}

// How far a chunk streams: a chunk by the water as far as its stones are seen.
float CoverRenderer::reach_of(const CityState &s, int64_t key) {
	auto it = wet_.find(key);
	if (it == wet_.end()) it = wet_.emplace(key, shore_near(s.terrain, key_x(key) * CHUNK, key_y(key) * CHUNK, CHUNK, shore_.search_m)).first;
	return (it->second ? shore_.reach_m : kit_.reach_m) + kit_.fade_m;
}

// The marked chunks drawn again (one whose result is on its way keeps its mark and is asked again when that lands; one not drawn
// is built fresh when it comes), the missing ones round the camera asked nearest first, and the far let go.
void CoverRenderer::offer(const CityState &s, const Vector3 &cam, std::vector<FrameAdmission::Unit> &out) {
	pending_ = 0;
	float reach = shore_.reach_m + kit_.fade_m;
	auto unit = [&](int64_t k, float d, int rank) {
		pending_++;
		out.push_back(FrameAdmission::Unit{ rank, d, 1, [this, &s, k, d]() {
					   pending_--;
					   submit(s, k, d);
				   } });
	};
	std::vector<int64_t> unmark;
	for (int64_t k : queue_.revs.wanted()) {
		if (queue_.revs.busy(k)) continue;
		if (chunks_.count(k) == 0) {
			unmark.push_back(k);
			continue;
		}
		unit(k, distance_to(k, cam), 1);
	}
	for (int64_t k : unmark) queue_.revs.unwant(k);
	for (auto it = chunks_.begin(); it != chunks_.end();) {
		if (distance_to(it->first, cam) <= reach_of(s, it->first) + DROP_CHUNKS * CHUNK) { ++it; continue; }
		queue_.drop(it->first);
		for (MultiMeshDraw &d : it->second) d.free();
		it = chunks_.erase(it);
	}
	int r = int(std::ceil(reach / CHUNK)), cx = int(std::floor(cam.x / CHUNK)), cz = int(std::floor(cam.z / CHUNK));
	for (int j = cz - r; j <= cz + r; j++)
		for (int i = cx - r; i <= cx + r; i++) {
			int64_t k = cell_key(i, j);
			float d = distance_to(k, cam);
			if (d > reach || chunks_.count(k) != 0) continue;
			if (d > reach_of(s, k) + CHUNK * SQRT2_F * HALF || !s.terrain.in_bounds((i + HALF) * CHUNK, (j + HALF) * CHUNK, -CHUNK * HALF)) continue;
			if (!queue_.revs.busy(k)) unit(k, d, 0);
		}
}

} // namespace city
