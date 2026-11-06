// The fall of a building's pieces (ruin_pieces.h).
#include "render/effects/ruin_pieces.h"

#include "core/job_pool.h"
#include "render/effects/mounds.h"
#include "render/render_layers.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr int MOST_FALLS = 24; // ruins drawn as pieces at once: the rest of a razed block falls as plain heaps
constexpr int64_t MOST_VERTICES = 6000000; // of them all, about 170 MB: past it the oldest at rest leaves its mound alone
constexpr int PIECES_A_ROW = 256; // of the matrix texture: building_body.gdshaderinc RUIN_PIECES_A_ROW
constexpr int TEXELS_A_PIECE = 3, FLOATS_A_TEXEL = 4, FLOATS_A_PIECE = TEXELS_A_PIECE * FLOATS_A_TEXEL;
constexpr float RESTING_M = 0.6f; // what starts this close to the ground rests on it (the 4.0): bonded to the world
constexpr float FORCE_SCALE = 5.f; // the 4.0's blows go 1..10, the bomb's force 1..50
constexpr float BLOW_RADIUS_M = 1.4f, BLOW_RADIUS_PER_FORCE_M = 0.55f; // how far a blow breaks, and the speed it throws with
constexpr float THROW_BASE_MS = 2.f, THROW_PER_FORCE_MS = 1.1f, THROW_SHARE_LEAST = 0.3f;
constexpr float BREAK_POWER_MS = 0.6f; // what falls from the break running on: it falls, it is not thrown
constexpr float THROW_LEAST = 0.5f, THROW_SPAN = 0.6f, THROW_UP = 0.25f;
constexpr float TURN_RANGE = 3.f, SPIN_BASE = 0.4f, SPIN_PER_POWER = 0.1f;
constexpr float HOP_S = 0.055f, HOP_LEAST = 0.7f, HOP_SPAN = 0.6f; // the break runs a bond in this long
constexpr float SUPPORT_EVERY_S = 0.1f;
constexpr float MOUND_FILL = 0.85f; // of a mound's height where the pieces come to rest: its shader's slumps average under it
constexpr float MOUND_SPREAD = 1.15f; // effect_ruins.cpp: it slumps past the walls it fell from
constexpr float BOX_SPREAD = 2.f; // of the building's height, how far a piece may end from it
constexpr int STEPS_MOST = 4;
constexpr float FARTHEST_M = 1e30f;
constexpr float LONGEST_FALL_S = 12.f;
constexpr int DUST_A_FRAME = 12, DUST_COUNT = 3;
constexpr float DUST_CHANCE = 0.5f, DUST_SIZE_M = 2.5f, DUST_LIFE_S = 3.5f, DUST_SPREAD = 0.6f;
constexpr float LAND_SPEED_MS = 3.f, LAND_RADIUS_M = 1.2f, LAND_CHANCE = 0.3f, LAND_LIFE_S = 4.f, LAND_SPREAD_M = 1.5f;
} // namespace

void RuinPieces::init(const RID &scenario, const Ref<ShaderMaterial> &material, const EffectParams &params) {
	scenario_ = scenario;
	material_ = material;
	law_ = params;
}

void RuinPieces::free_fall(Fall &f) {
	RenderingServer *rs = RenderingServer::get_singleton();
	if (f.inst.is_valid()) rs->free_rid(f.inst);
	if (f.texture.is_valid()) rs->free_rid(f.texture);
	f.inst = RID();
	f.texture = RID();
}

void RuinPieces::reset() {
	for (auto &[seed, f] : falls_) free_fall(f);
	falls_.clear();
	inbox_ = std::make_shared<Inbox>(); // what a job still has in hand arrives nowhere
}

void RuinPieces::keep_only(const std::vector<Ruin> &ruins) {
	for (auto it = falls_.begin(); it != falls_.end();) {
		const uint32_t seed = it->first;
		if (std::any_of(ruins.begin(), ruins.end(), [seed](const Ruin &r) { return r.seed == seed; })) {
			++it;
			continue;
		}
		free_fall(it->second);
		it = falls_.erase(it);
	}
}

bool RuinPieces::fall(const CityState &s, const Ruin &r, const Blow &blow) {
	if (!maker_ || !material_.is_valid() || !s.jobs || falls_.count(r.seed)) return false;
	if (int(falls_.size()) >= MOST_FALLS) { // the oldest at rest makes way; with all of them still coming down this one is a plain heap
		auto rest = std::find_if(falls_.begin(), falls_.end(), [](const auto &e) { return e.second.ready && e.second.left == 0; });
		if (rest == falls_.end()) return false;
		free_fall(rest->second);
		falls_.erase(rest);
	}
	Fall &f = falls_[r.seed];
	f.ruin = r;
	f.blow = blow;
	f.ground = s.terrain.height_at(r.x, r.z);
	const float reach = 0.5f * std::hypot(r.w, r.d) + 1.f;
	auto ground = std::make_shared<HeightPatch>(s.terrain.patch(r.x - reach, r.z - reach, r.x + reach, r.z + reach));
	const b40::Data *data = &s.buildings.designs;
	s.jobs->submit(JobKey{ JobLane::RUINS, int64_t(r.seed) }, 0.f, [inbox = inbox_, maker = maker_, data, ground, record = r.record, seed = r.seed]() {
		std::shared_ptr<RuinMesh> cut = maker(*data, record, *ground);
		std::lock_guard<std::mutex> lk(inbox->mutex);
		inbox->done.emplace_back(seed, std::move(cut));
	});
	return true;
}

void RuinPieces::make_room(int64_t vertices) {
	int64_t held = vertices;
	for (const auto &[seed, f] : falls_)
		if (f.cut) held += f.cut->vertices;
	for (auto it = falls_.begin(); it != falls_.end() && held > MOST_VERTICES;) {
		if (!it->second.ready || it->second.left > 0) {
			++it;
			continue;
		}
		held -= it->second.cut->vertices;
		free_fall(it->second);
		it = falls_.erase(it);
	}
}

// The mesh on its instance, the pieces standing as they stood.
void RuinPieces::adopt(Fall &f, const std::shared_ptr<RuinMesh> &cut) {
	make_room(cut->vertices);
	f.cut = cut;
	const int n = int(cut->pieces.size());
	f.pieces.resize(size_t(n));
	for (int i = 0; i < n; i++) {
		const RuinPiece &src = cut->pieces[size_t(i)];
		Piece &p = f.pieces[size_t(i)];
		p.home = src.centre;
		p.p = src.centre;
		p.radius = src.radius;
		p.first_link = src.first_link;
		p.links = src.links;
		p.grounded = src.low <= f.ground + RESTING_M;
		p.asleep = true;
		p.shape(src.half, 4.f * src.half.length_squared());
	}
	f.rows_of_pieces = (n + PIECES_A_ROW - 1) / PIECES_A_ROW;
	f.rows.resize(int64_t(PIECES_A_ROW) * f.rows_of_pieces * FLOATS_A_PIECE * int64_t(sizeof(float)));
	for (int i = 0; i < n; i++) write(f, size_t(i));
	RenderingServer *rs = RenderingServer::get_singleton();
	const Ref<Image> image = Image::create_from_data(PIECES_A_ROW * TEXELS_A_PIECE, f.rows_of_pieces, false, Image::FORMAT_RGBAF, f.rows);
	f.texture = rs->texture_2d_create(image);
	f.material = material_->duplicate();
	rs->material_set_param(f.material->get_rid(), "pieces", f.texture);
	const RID mesh = cut->mesh.rid;
	rs->mesh_surface_set_material(mesh, 0, f.material->get_rid());
	f.inst = rs->instance_create2(mesh, scenario_);
	rs->instance_set_layer_mask(f.inst, LAYER_BASE);
	const Vector3 out = Vector3(1, 0, 1) * ((cut->hi.y - cut->lo.y) * BOX_SPREAD);
	rs->instance_set_custom_aabb(f.inst, AABB(cut->lo - out, cut->hi - cut->lo + out * 2.f));
	schedule(f);
	f.ready = true;
}

// When each piece goes: what the blow reaches at once, thrown, then the break running on by the bonds from there.
void RuinPieces::schedule(Fall &f) {
	const float force = f.blow.force / FORCE_SCALE;
	const float reach = BLOW_RADIUS_M + force * BLOW_RADIUS_PER_FORCE_M, power = THROW_BASE_MS + force * THROW_PER_FORCE_MS;
	stack_.clear();
	int nearest = 0;
	float closest = FARTHEST_M;
	for (size_t i = 0; i < f.pieces.size(); i++) {
		Piece &p = f.pieces[i];
		const float d = p.home.distance_to(f.blow.at) - p.radius * 0.5f;
		if (d < closest) {
			closest = d;
			nearest = int(i);
		}
		if (d >= reach) continue;
		p.scheduled = true;
		p.power = power * std::max(THROW_SHARE_LEAST, 1.f - d / reach);
		stack_.push_back(int(i));
	}
	if (stack_.empty()) { // brought down from afar: it goes from the side the blow came from
		f.pieces[size_t(nearest)].scheduled = true;
		stack_.push_back(nearest);
	}
	for (size_t head = 0; head < stack_.size(); head++) { // a breadth first walk over the bonds
		const Piece &from = f.pieces[size_t(stack_[head])];
		for (int l = 0; l < from.links; l++) {
			const int next = f.cut->links[size_t(from.first_link + l)];
			Piece &p = f.pieces[size_t(next)];
			if (p.scheduled) continue;
			p.scheduled = true;
			p.fall_at = from.fall_at + HOP_S * (HOP_LEAST + rng_.f() * HOP_SPAN);
			stack_.push_back(next);
		}
	}
	f.left = int(f.pieces.size());
}

void RuinPieces::let_go(Fall &f, Piece &p, const Vector3 &from, float power, Puffs &puffs) {
	if (p.loose) return;
	p.loose = true;
	p.asleep = false;
	p.rest = 0;
	const Vector3 away = (p.p - from).normalized();
	p.v = away * (power * (THROW_LEAST + rng_.f() * THROW_SPAN));
	p.v.y += power * THROW_UP * rng_.f();
	const float spin = SPIN_BASE + power * SPIN_PER_POWER;
	p.w = Vector3(rng_.f() - 0.5f, rng_.f() - 0.5f, rng_.f() - 0.5f) * (TURN_RANGE * spin);
	f.dirty = true;
	if (dust_left_ > 0 && rng_.f() < DUST_CHANCE) {
		dust_left_--;
		puffs.burst(p.p.x, p.p.y, p.p.z, DUST_COUNT, PUFF_CONCRETE, p.radius * DUST_SPREAD, DUST_SIZE_M, DUST_LIFE_S);
	}
}

// Every standing piece no longer joined to the ground through standing pieces falls.
void RuinPieces::drop_unsupported(Fall &f, Puffs &puffs) {
	held_.assign(f.pieces.size(), 0);
	stack_.clear();
	for (size_t i = 0; i < f.pieces.size(); i++)
		if (!f.pieces[i].loose && f.pieces[i].grounded) {
			held_[i] = 1;
			stack_.push_back(int(i));
		}
	while (!stack_.empty()) {
		const Piece &p = f.pieces[size_t(stack_.back())];
		stack_.pop_back();
		for (int l = 0; l < p.links; l++) {
			const int next = f.cut->links[size_t(p.first_link + l)];
			if (held_[size_t(next)] || f.pieces[size_t(next)].loose) continue;
			held_[size_t(next)] = 1;
			stack_.push_back(next);
		}
	}
	for (size_t i = 0; i < f.pieces.size(); i++)
		if (!f.pieces[i].loose && !held_[i]) let_go(f, f.pieces[i], f.pieces[i].p + Vector3(0, 1, 0), BREAK_POWER_MS, puffs);
}

// The height of the mound the pieces rest on, as Mounds draws it.
float RuinPieces::top(const Fall &f, float x, float z) const {
	const Ruin &r = f.ruin;
	const float c = std::cos(r.rot), s = std::sin(r.rot), dx = x - r.x, dz = z - r.z;
	const float u = (dx * c - dz * s) / (r.w * MOUND_SPREAD), v = (dx * s + dz * c) / (r.d * MOUND_SPREAD);
	if (std::fabs(u) >= 0.5f || std::fabs(v) >= 0.5f) return f.ground;
	return f.ground + f.blow.heap * MOUND_FILL * Mounds::hill_at(u, v);
}

void RuinPieces::step(Fall &f, Puffs &puffs) {
	f.clock += RIGID_STEP_S;
	f.support_in -= RIGID_STEP_S;
	for (Piece &p : f.pieces) {
		const bool stays = p.grounded && p.power <= 0.f; // what stands on the ground stays, unless the blow itself threw it
		if (!p.loose && !stays && f.clock >= p.fall_at) let_go(f, p, f.blow.at, p.power > 0.f ? p.power : BREAK_POWER_MS, puffs);
	}
	if (f.support_in <= 0.f) {
		f.support_in = SUPPORT_EVERY_S;
		drop_unsupported(f, puffs);
	}
	auto ground = [this, &f](float x, float z) { return top(f, x, z); };
	f.left = 0;
	for (size_t i = 0; i < f.pieces.size(); i++) {
		Piece &p = f.pieces[i];
		if (!p.loose) {
			f.left += p.grounded ? 0 : 1;
			continue;
		}
		if (p.asleep) continue;
		f.left++;
		const float falling = p.v.y;
		rigid_step(p, ground, law_);
		p.aloft += RIGID_STEP_S;
		if (p.aloft > LONGEST_FALL_S) p.asleep = true; // on a mound's slope a plate rocks for good: nothing else stops it
		write(f, i);
		f.dirty = true;
		if (falling < -LAND_SPEED_MS && p.v.y > falling * 0.5f && p.radius > LAND_RADIUS_M && dust_left_ > 0 && rng_.f() < LAND_CHANCE) {
			dust_left_--;
			puffs.burst(p.p.x, p.p.y, p.p.z, DUST_COUNT, PUFF_CONCRETE, LAND_SPREAD_M, DUST_SIZE_M, LAND_LIFE_S);
		}
	}
}

void RuinPieces::write(Fall &f, size_t i) {
	const Piece &p = f.pieces[i];
	const Basis b(p.q);
	const Vector3 at = p.p - b.xform(p.home);
	float *o = reinterpret_cast<float *>(f.rows.ptrw()) + i * FLOATS_A_PIECE;
	for (int r = 0; r < TEXELS_A_PIECE; r++) {
		o[r * FLOATS_A_TEXEL] = b.rows[r].x;
		o[r * FLOATS_A_TEXEL + 1] = b.rows[r].y;
		o[r * FLOATS_A_TEXEL + 2] = b.rows[r].z;
		o[r * FLOATS_A_TEXEL + 3] = at[r];
	}
}

void RuinPieces::upload(Fall &f) {
	const Ref<Image> image = Image::create_from_data(PIECES_A_ROW * TEXELS_A_PIECE, f.rows_of_pieces, false, Image::FORMAT_RGBAF, f.rows);
	RenderingServer::get_singleton()->texture_2d_update(f.texture, image, 0);
	f.dirty = false;
}

void RuinPieces::update(float dt, Puffs &puffs) {
	{
		std::lock_guard<std::mutex> lk(inbox_->mutex);
		for (auto &[seed, cut] : inbox_->done) {
			auto it = falls_.find(seed);
			if (it == falls_.end()) continue;
			if (cut && !cut->pieces.empty()) {
				adopt(it->second, cut);
				continue;
			}
			uncut_.push_back(it->second.ruin);
			falls_.erase(it);
		}
		inbox_->done.clear();
	}
	dust_left_ = DUST_A_FRAME;
	clock_ = std::min(clock_ + dt, RIGID_STEP_S * float(STEPS_MOST));
	while (clock_ >= RIGID_STEP_S) {
		clock_ -= RIGID_STEP_S;
		for (auto &[seed, f] : falls_)
			if (f.ready && f.left > 0) step(f, puffs);
	}
	for (auto &[seed, f] : falls_)
		if (f.ready && f.dirty) upload(f);
}

std::vector<Ruin> RuinPieces::take_uncut() {
	std::vector<Ruin> out;
	out.swap(uncut_);
	return out;
}

bool RuinPieces::moving() const {
	return std::any_of(falls_.begin(), falls_.end(), [](const auto &e) { return !e.second.ready || e.second.left > 0; });
}

int RuinPieces::pieces() const {
	int n = 0;
	for (const auto &[seed, f] : falls_) n += int(f.pieces.size());
	return n;
}

int RuinPieces::flying() const {
	int n = 0;
	for (const auto &[seed, f] : falls_)
		for (const Piece &p : f.pieces) n += p.loose && !p.asleep ? 1 : 0;
	return n;
}

} // namespace city
