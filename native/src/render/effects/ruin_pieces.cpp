// A building's pieces, drawn (ruin_pieces.h).
#include "render/effects/ruin_pieces.h"

#include "core/job_pool.h"
#include "render/render_layers.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>
#include <iterator>

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
	if (broken_.erase(f.ruin.seed)) repaint_.push_back(chunk_of(f.ruin.x, f.ruin.z));
	if (f.inst.is_valid()) rs->free_rid(f.inst);
	if (f.texture.is_valid()) rs->free_rid(f.texture);
	f.inst = RID();
	f.texture = RID();
}

void RuinPieces::reset() {
	for (auto &[seed, f] : falls_) free_fall(f);
	falls_.clear();
	repaint_.clear();
	inbox_ = std::make_shared<Inbox>(); // what a job still has in hand arrives nowhere
}

void RuinPieces::keep_only(const std::vector<Ruin> &ruins, const std::map<uint32_t, size_t> &scarred) {
	for (auto it = falls_.begin(); it != falls_.end();) {
		const uint32_t seed = it->first;
		const Fall &f = it->second;
		const auto mine = scarred.find(seed);
		const bool stays = f.standing ? mine != scarred.end() && mine->second == f.scars.size()
				: std::any_of(ruins.begin(), ruins.end(), [seed](const Ruin &r) { return r.seed == seed; });
		if (stays) {
			++it;
			continue;
		}
		free_fall(it->second);
		it = falls_.erase(it);
	}
}

bool RuinPieces::add(const CityState &s, const Ruin &r, float heap, bool falling) {
	const auto held = falls_.find(r.seed);
	if (held != falls_.end() && held->second.standing) { // what was left of it comes down
		Fall &f = held->second;
		f.standing = false;
		f.falling = falling;
		f.ruin = r;
		broken_.erase(r.seed);
		if (f.ready) f.sim->collapse(r, heap);
		else f.heap_after = heap;
		return true;
	}
	if (!maker_ || !material_.is_valid() || !s.jobs || held != falls_.end() || !room()) return false;
	Fall &f = falls_[r.seed];
	f.ruin = r;
	f.falling = falling;
	start(s, f, heap);
	return true;
}

void RuinPieces::scar(const CityState &s, const Ruin &r, const std::vector<Scar> &scars, bool falling) {
	const auto held = falls_.find(r.seed);
	if (held != falls_.end()) {
		Fall &f = held->second;
		f.scars = scars;
		f.falling = f.falling || falling;
		if (!f.ready) return; // the job has the first of them: the rest wait for its pieces (adopt)
		for (; f.scars_done < scars.size(); f.scars_done++) f.sim->scar(scars[f.scars_done]);
		return;
	}
	if (!maker_ || !material_.is_valid() || !s.jobs || !room()) return;
	Fall &f = falls_[r.seed];
	f.ruin = r;
	f.falling = falling;
	f.standing = true;
	f.scars = scars;
	start(s, f, 0.f);
}

std::vector<int64_t> RuinPieces::take_repaint() {
	std::vector<int64_t> out;
	out.swap(repaint_);
	return out;
}

bool RuinPieces::room() {
	if (int(falls_.size()) < MOST_FALLS) return true;
	// The oldest at rest makes way, a heap before a building still standing: that one would come back whole. With all of them still
	// coming down this one is a plain heap.
	auto at_rest = [](const auto &e) { return e.second.ready && e.second.sim->resting(); };
	auto rest = std::find_if(falls_.begin(), falls_.end(), [&](const auto &e) { return at_rest(e) && !e.second.standing; });
	if (rest == falls_.end()) rest = std::find_if(falls_.begin(), falls_.end(), at_rest);
	if (rest == falls_.end()) return false;
	free_fall(rest->second);
	falls_.erase(rest);
	return true;
}

void RuinPieces::start(const CityState &s, Fall &f, float heap) {
	const Ruin &r = f.ruin;
	const bool falling = f.falling, standing = f.standing;
	const std::vector<Scar> scars = f.scars;
	const float reach = 0.5f * std::hypot(r.w, r.d) + LOT_PAD_M, ground = s.terrain.height_at(r.x, r.z);
	auto patch = std::make_shared<HeightPatch>(s.terrain.patch(r.x - reach, r.z - reach, r.x + reach, r.z + reach));
	const b40::Data *data = &s.buildings.designs;
	s.jobs->submit(JobKey{ JobLane::RUINS, int64_t(r.seed) }, 0.f, [inbox = inbox_, maker = maker_, law = law_, data, patch, r, scars, standing, ground, heap, falling]() {
		Done done;
		done.seed = r.seed;
		done.cut = maker(*data, r.record, *patch);
		if (done.cut && !done.cut->pieces.empty()) {
			done.sim = std::make_shared<RuinSim>();
			done.sim->begin(done.cut, r, ground, heap, law, standing);
			for (const Scar &c : scars) {
				done.sim->scar(c);
				if (!falling) done.sim->settle();
			}
			done.scars_done = scars.size();
			if (!falling) done.sim->settle();
		}
		std::lock_guard<std::mutex> lk(inbox->mutex);
		inbox->done.push_back(std::move(done));
	});
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

namespace {
int rows_of(int pieces) { return (pieces + PIECES_A_ROW - 1) / PIECES_A_ROW; }
int64_t rows_bytes(int rows) { return int64_t(PIECES_A_ROW) * rows * RuinSim::FLOATS_A_PIECE * int64_t(sizeof(float)); }
} // namespace

// The mesh on its instance, the pieces as the sim has them.
void RuinPieces::adopt(Fall &f, const Done &done) {
	make_room(done.cut->vertices);
	f.cut = done.cut;
	f.sim = done.sim;
	const int n = f.sim->pieces();
	f.rows_of_pieces = rows_of(n);
	f.rows.resize(rows_bytes(f.rows_of_pieces));
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
	f.scars_done = done.scars_done;
	for (; f.scars_done < f.scars.size(); f.scars_done++) f.sim->scar(f.scars[f.scars_done]);
	if (f.heap_after >= 0.f) f.sim->collapse(f.ruin, f.heap_after);
	if (f.standing && broken_.insert(f.ruin.seed).second) repaint_.push_back(chunk_of(f.ruin.x, f.ruin.z));
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

// The cuts the jobs finished, in the order they came: each adopted while the frame's admission has time and takes its texture's bytes;
// the first it does not waits with all after it for the next frame. What hands the server nothing (a ruin gone since, a design with no
// walls) never waits.
void RuinPieces::take_done(FrameAdmission &admission) {
	const std::shared_ptr<Inbox> inbox = inbox_;
	std::vector<Done> done;
	{
		std::lock_guard<std::mutex> lk(inbox->mutex);
		done.swap(inbox->done);
	}
	size_t next = 0;
	for (; next < done.size(); next++) {
		const Done &d = done[next];
		auto it = falls_.find(d.seed);
		if (it == falls_.end()) continue;
		if (!d.sim) {
			if (!it->second.standing) uncut_.push_back(Uncut{ it->second.ruin, it->second.falling }); // a design of its own stands whole
			falls_.erase(it);
			continue;
		}
		if (!admission.due() || !admission.upload.take(rows_bytes(rows_of(d.sim->pieces())))) break;
		adopt(it->second, d);
	}
	if (next == done.size()) return;
	std::lock_guard<std::mutex> lk(inbox->mutex); // ahead of what came meanwhile
	inbox->done.insert(inbox->done.begin(), std::make_move_iterator(done.begin() + std::ptrdiff_t(next)), std::make_move_iterator(done.end()));
}

void RuinPieces::update(float dt, Puffs &puffs, FrameAdmission &admission) {
	take_done(admission);
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
