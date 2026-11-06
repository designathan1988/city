// A building's pieces, drawn (ruin_pieces.h).
#include "render/effects/ruin_pieces.h"

#include "core/job_pool.h"
#include "render/render_layers.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int MOST_FALLS = 24; // ruins drawn as pieces at once: the rest of a razed block falls as plain heaps
constexpr int64_t MOST_VERTICES = 6000000; // of them all, about 170 MB: past it the oldest at rest leaves its mound alone
constexpr int PIECES_A_ROW = 256; // of the matrix texture: building_body.gdshaderinc RUIN_PIECES_A_ROW
constexpr int TEXELS_A_PIECE = 3;
constexpr float BOX_SPREAD = 2.f; // of the building's height, how far a piece may end from it
constexpr int STEPS_MOST = 4;
constexpr int DUST_A_FRAME = 12, DUST_COUNT = 3;
constexpr float DUST_CHANCE = 0.5f, DUST_SIZE_M = 2.5f, DUST_LIFE_S = 3.5f, DUST_SPREAD = 0.6f;
constexpr float LAND_RADIUS_M = 1.2f, LAND_CHANCE = 0.3f, LAND_LIFE_S = 4.f, LAND_SPREAD_M = 1.5f;
constexpr float LOT_PAD_M = 1.f;
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

bool RuinPieces::add(const CityState &s, const Ruin &r, float heap, bool falling) {
	if (!maker_ || !material_.is_valid() || !s.jobs || falls_.count(r.seed)) return false;
	if (int(falls_.size()) >= MOST_FALLS) { // the oldest at rest makes way; with all of them still coming down this one is a plain heap
		auto rest = std::find_if(falls_.begin(), falls_.end(), [](const auto &e) { return e.second.ready && e.second.sim->resting(); });
		if (rest == falls_.end()) return false;
		free_fall(rest->second);
		falls_.erase(rest);
	}
	Fall &f = falls_[r.seed];
	f.ruin = r;
	f.falling = falling;
	const float reach = 0.5f * std::hypot(r.w, r.d) + LOT_PAD_M, ground = s.terrain.height_at(r.x, r.z);
	auto patch = std::make_shared<HeightPatch>(s.terrain.patch(r.x - reach, r.z - reach, r.x + reach, r.z + reach));
	const b40::Data *data = &s.buildings.designs;
	s.jobs->submit(JobKey{ JobLane::RUINS, int64_t(r.seed) }, 0.f, [inbox = inbox_, maker = maker_, law = law_, data, patch, r, ground, heap, falling]() {
		Done done;
		done.seed = r.seed;
		done.cut = maker(*data, r.record, *patch);
		if (done.cut && !done.cut->pieces.empty()) {
			done.sim = std::make_shared<RuinSim>();
			done.sim->begin(done.cut, r, ground, heap, law);
			if (!falling) done.sim->settle();
		}
		std::lock_guard<std::mutex> lk(inbox->mutex);
		inbox->done.push_back(std::move(done));
	});
	return true;
}

void RuinPieces::make_room(int64_t vertices) {
	int64_t held = vertices;
	for (const auto &[seed, f] : falls_)
		if (f.cut) held += f.cut->vertices;
	for (auto it = falls_.begin(); it != falls_.end() && held > MOST_VERTICES;) {
		if (!it->second.ready || !it->second.sim->resting()) {
			++it;
			continue;
		}
		held -= it->second.cut->vertices;
		free_fall(it->second);
		it = falls_.erase(it);
	}
}

// The mesh on its instance, the pieces as the sim has them.
void RuinPieces::adopt(Fall &f, const Done &done) {
	make_room(done.cut->vertices);
	f.cut = done.cut;
	f.sim = done.sim;
	const int n = f.sim->pieces();
	f.rows_of_pieces = (n + PIECES_A_ROW - 1) / PIECES_A_ROW;
	f.rows.resize(int64_t(PIECES_A_ROW) * f.rows_of_pieces * RuinSim::FLOATS_A_PIECE * int64_t(sizeof(float)));
	float *out = reinterpret_cast<float *>(f.rows.ptrw());
	for (int i = 0; i < n; i++) f.sim->write(size_t(i), out + i * RuinSim::FLOATS_A_PIECE);
	RenderingServer *rs = RenderingServer::get_singleton();
	f.texture = rs->texture_2d_create(Image::create_from_data(PIECES_A_ROW * TEXELS_A_PIECE, f.rows_of_pieces, false, Image::FORMAT_RGBAF, f.rows));
	f.material = material_->duplicate();
	rs->material_set_param(f.material->get_rid(), "pieces", f.texture);
	const RID mesh = f.cut->mesh.rid;
	rs->mesh_surface_set_material(mesh, 0, f.material->get_rid());
	f.inst = rs->instance_create2(mesh, scenario_);
	rs->instance_set_layer_mask(f.inst, LAYER_BASE);
	const Vector3 out_box = Vector3(1, 0, 1) * ((f.cut->hi.y - f.cut->lo.y) * BOX_SPREAD);
	rs->instance_set_custom_aabb(f.inst, AABB(f.cut->lo - out_box, f.cut->hi - f.cut->lo + out_box * 2.f));
	f.ready = true;
}

void RuinPieces::dust(const std::vector<RuinSim::Note> &notes, Puffs &puffs) {
	for (const RuinSim::Note &n : notes) {
		if (dust_left_ <= 0) return;
		if (n.landing && (n.radius <= LAND_RADIUS_M || rng_.f() >= LAND_CHANCE)) continue;
		if (!n.landing && rng_.f() >= DUST_CHANCE) continue;
		dust_left_--;
		puffs.burst(n.at.x, n.at.y, n.at.z, DUST_COUNT, PUFF_CONCRETE, n.landing ? LAND_SPREAD_M : n.radius * DUST_SPREAD, DUST_SIZE_M, n.landing ? LAND_LIFE_S : DUST_LIFE_S);
	}
}

void RuinPieces::upload(Fall &f) {
	float *out = reinterpret_cast<float *>(f.rows.ptrw());
	for (int i = 0; i < f.sim->pieces(); i++)
		if (f.sim->moved(size_t(i))) f.sim->write(size_t(i), out + i * RuinSim::FLOATS_A_PIECE);
	f.sim->clear_moved();
	const Ref<Image> image = Image::create_from_data(PIECES_A_ROW * TEXELS_A_PIECE, f.rows_of_pieces, false, Image::FORMAT_RGBAF, f.rows);
	RenderingServer::get_singleton()->texture_2d_update(f.texture, image, 0);
	f.dirty = false;
}

void RuinPieces::update(float dt, Puffs &puffs) {
	{
		std::lock_guard<std::mutex> lk(inbox_->mutex);
		for (const Done &d : inbox_->done) {
			auto it = falls_.find(d.seed);
			if (it == falls_.end()) continue;
			if (d.sim) {
				adopt(it->second, d);
				continue;
			}
			uncut_.push_back(Uncut{ it->second.ruin, it->second.falling });
			falls_.erase(it);
		}
		inbox_->done.clear();
	}
	dust_left_ = DUST_A_FRAME;
	clock_ = std::min(clock_ + dt, RIGID_STEP_S * float(STEPS_MOST));
	while (clock_ >= RIGID_STEP_S) {
		clock_ -= RIGID_STEP_S;
		notes_.clear();
		for (auto &[seed, f] : falls_)
			if (f.ready && !f.sim->resting()) {
				f.sim->step(&notes_);
				f.dirty = true;
			}
		dust(notes_, puffs);
	}
	for (auto &[seed, f] : falls_)
		if (f.ready && f.dirty) upload(f);
}

std::vector<RuinPieces::Uncut> RuinPieces::take_uncut() {
	std::vector<Uncut> out;
	out.swap(uncut_);
	return out;
}

bool RuinPieces::moving() const {
	return std::any_of(falls_.begin(), falls_.end(), [](const auto &e) { return !e.second.ready || !e.second.sim->resting(); });
}

int RuinPieces::pieces() const {
	int n = 0;
	for (const auto &[seed, f] : falls_) n += f.ready ? f.sim->pieces() : 0;
	return n;
}

int RuinPieces::flying() const {
	int n = 0;
	for (const auto &[seed, f] : falls_) n += f.ready ? f.sim->flying() : 0;
	return n;
}

uint32_t RuinPieces::digest(uint32_t seed) const {
	const auto it = falls_.find(seed);
	return it == falls_.end() || !it->second.ready || !it->second.sim->resting() ? 0 : it->second.sim->digest();
}

} // namespace city
