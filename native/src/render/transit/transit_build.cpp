// The job that makes a track or a stop of the transit plan (transit_renderer.h): the item's plan copied with the map held, the city
// under it cut a stretch at a time, its boxes laid from the cuts and its meshes baked, all on a worker with the map let go.
#include "render/transit/transit_renderer.h"

#include "core/grid.h"
#include "render/surface_data.h"
#include "sim/transit/transit_ground.h"

#include <algorithm>
#include <limits>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f;
} // namespace

void TransitRenderer::add_surface(TypedArray<Dictionary> &surfaces, const MeshBuilder &mb, const RID &material) {
	if (!mb.empty()) surfaces.push_back(mb.surface(material));
}

// The item's plan as it stands now: its track or stop copied, with what the stop's boxes need of the rest of the plan.
TransitRenderer::Plan TransitRenderer::plan_of(const CityState &live, int64_t key) const {
	Plan plan;
	const int id = int(key / KEY_KINDS);
	plan.street = live.catalog.widest_half() + live.terrain.spacing; // transit_paved's reach, and a cell of ground round a point
	if (key % KEY_KINDS == 0) {
		for (const RailTrack &t : live.transit.tracks)
			if (t.id == id) {
				plan.kind = Plan::Kind::TRACK;
				plan.track = t;
			}
	} else if (const TransitStop *q = live.transit.stop(id)) {
		plan.kind = Plan::Kind::STOP;
		plan.stop = *q;
		if (const RailTrack *t = transit_track_of(live, *q)) {
			plan.on_track = true;
			plan.track = *t;
		}
		plan.tint = transit_line_colour(live.transit, look_, id);
	}
	return plan;
}

void TransitRenderer::build(const CityState &s, int64_t key, Built &b) const {
	Plan plan;
	{
		MapRead map(s); // for the copy of the item's plan alone
		plan = plan_of(s, key);
	}
	b.x0 = b.z0 = std::numeric_limits<float>::max();
	b.x1 = b.z1 = std::numeric_limits<float>::lowest();
	const auto reach = [&](const V2 &p, float pad) {
		b.x0 = std::min(b.x0, p.x - pad);
		b.z0 = std::min(b.z0, p.y - pad);
		b.x1 = std::max(b.x1, p.x + pad);
		b.z1 = std::max(b.z1, p.y + pad);
	};
	if (plan.kind == Plan::Kind::TRACK) {
		// A stretch at a time, each from a cut of the streets and the ground under it: a long track never copies the box of its whole run.
		const std::vector<CutBox> stretches = transit_track_stretches(look_, plan.track);
		b.parts.resize(stretches.size());
		for (size_t i = 0; i < stretches.size(); i++) {
			const CityCut cut = cut_city(s, CUT_ROADS, [&](const CityState &) { return stretches[i].grown(plan.street); });
			transit_track_stretch(cut, look_, plan.track, i, b.parts[i]);
		}
		for (const V2 &p : plan.track.points) reach(p, look_.ballast.wide);
	} else if (plan.kind == Plan::Kind::STOP) {
		// The stop's street and the lots round it, as far as a metro's way down is looked for.
		const TransitStop &q = plan.stop;
		CutBox box{ q.at.x, q.at.y, q.at.x, q.at.y };
		if (q.has_entrance) box.take(q.entrance.x, q.entrance.y, q.entrance.x, q.entrance.y);
		const CityCut cut = cut_city(s, CUT_ROADS | CUT_BUILDINGS, [&](const CityState &) { return box.grown(plan.street + look_.entrance_reach + look_.entrance_clear); });
		b.parts.resize(1);
		transit_stop_parts(cut, look_, q, plan.on_track ? &plan.track : nullptr, plan.tint, b.parts[0]);
		reach(q.at, CHUNK * HALF);
	}
}

int64_t TransitRenderer::Built::bytes() const {
	int64_t n = 0;
	for (const TransitMeshes &m : parts) {
		n += m.solid.bytes() + m.glass.bytes();
		for (const MeshBuilder &x : m.xray) n += x.bytes();
	}
	return n;
}

void TransitRenderer::Built::bake() {
	meshes.reserve(parts.size());
	for (const TransitMeshes &m : parts) meshes.push_back(owner->bake_piece(m));
	parts.clear();
	baked = true;
}

// One stretch's meshes, each whole with its materials: the body (opaque and see-through) and the cut drawn through the ground.
std::pair<OwnedMesh, OwnedMesh> TransitRenderer::bake_piece(const TransitMeshes &m) const {
	TypedArray<Dictionary> solid, cut;
	add_surface(solid, m.solid, material_);
	add_surface(solid, m.glass, glass_);
	for (int i = 0; i < TRANSIT_XRAY_LAYERS; i++) add_surface(cut, m.xray[i], xray_[i]);
	return { OwnedMesh::of(solid.is_empty() ? RID() : create_mesh(solid)), OwnedMesh::of(cut.is_empty() ? RID() : create_mesh(cut)) };
}

void TransitRenderer::submit(const CityState &s, int64_t key, float priority) {
	queue_.set_pool(s.jobs.get());
	const uint64_t ticket = queue_.revs.ask(key);
	const JobKey job{ JobLane::TRANSIT, key };
	s.jobs->submit(job, priority, [this, &s, key, job, ticket, priority]() {
		auto b = std::make_shared<Built>();
		b->key = key;
		b->job = job;
		b->ticket = ticket;
		b->priority = priority;
		b->owner = this;
		build(s, key, *b);
		queue_.push(std::move(b));
	});
}

} // namespace city
