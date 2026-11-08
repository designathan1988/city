// The fall of a building's pieces (ruin_sim.h).
#include "render/effects/ruin_sim.h"

#include "render/effects/mounds.h"

#include <algorithm>
#include <cmath>
#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr float RESTING_M = 0.6f; // what starts this close to the ground rests on it (the 4.0): bonded to the world
constexpr float THROW_BASE_MS = 2.f, THROW_PER_FORCE_MS = 1.1f, THROW_SHARE_LEAST = 0.3f;
constexpr float BREAK_POWER_MS = 0.6f; // what falls from the break running on: it falls, it is not thrown
constexpr float THROW_LEAST = 0.5f, THROW_SPAN = 0.6f, THROW_UP = 0.25f;
constexpr float TURN_RANGE = 3.f, SPIN_BASE = 0.4f, SPIN_PER_POWER = 0.1f;
constexpr float HOP_S = 0.055f, HOP_LEAST = 0.7f, HOP_SPAN = 0.6f; // the break runs a bond in this long
constexpr float SUPPORT_EVERY_S = 0.1f;
constexpr float MOUND_FILL = 0.85f; // of a mound's height where the pieces come to rest: its shader's slumps average under it
constexpr float MOUND_SPREAD = 1.15f; // effect_ruins.cpp: it slumps past the walls it fell from
constexpr float LONGEST_ROCK_S = 12.f; // a piece still rocking this long after it first came down is laid to rest where it is
constexpr float LAND_SPEED_MS = 3.f;
constexpr float FARTHEST_M = 1e30f;
constexpr float PILE_CELL_M = 0.5f, PILE_OVER = 2.f; // of the lot's reach, what the pile map covers
constexpr int PILE_MOST_SIDE = 256;
constexpr float FAR_DOWN_M = -1e9f;
constexpr int SETTLE_MOST_STEPS = 1200; // twenty seconds
} // namespace

float RuinSim::Pile::at(float x, float z) const {
	const int i = int((x - from.x) / cell), j = int((z - from.y) / cell);
	if (i < 0 || j < 0 || i >= side || j >= side) return FAR_DOWN_M;
	return top[size_t(j) * size_t(side) + size_t(i)];
}

void RuinSim::Pile::lay(const Vector3 &lo, const Vector3 &hi, float height) {
	const int i0 = std::max(int((lo.x - from.x) / cell), 0), i1 = std::min(int((hi.x - from.x) / cell), side - 1);
	const int j0 = std::max(int((lo.z - from.y) / cell), 0), j1 = std::min(int((hi.z - from.y) / cell), side - 1);
	for (int j = j0; j <= j1; j++)
		for (int i = i0; i <= i1; i++) {
			float &t = top[size_t(j) * size_t(side) + size_t(i)];
			t = std::max(t, height);
		}
}

void RuinSim::begin(const std::shared_ptr<RuinMesh> &cut, const Ruin &ruin, float ground, float heap, const EffectParams &law, bool standing) {
	cut_ = cut;
	ruin_ = ruin;
	ground_ = ground;
	heap_ = heap;
	law_ = law;
	rng_ = Rng(uint64_t(ruin.seed) + 1);
	cos_ = std::cos(ruin.rot);
	sin_ = std::sin(ruin.rot);
	// The pile keeps its cell (a coarser one is raised whole by any piece over part of it, and the heap of a big building climbs into the
	// air): a big building's map covers what the grid reaches round its middle, and what lands past it lands on the ground.
	const float reach = std::min(std::max(ruin.w, ruin.d) * PILE_OVER, PILE_CELL_M * float(PILE_MOST_SIDE - 1) * 0.5f);
	pile_.cell = PILE_CELL_M;
	pile_.side = int(2.f * reach / pile_.cell) + 1;
	pile_.from = Vector2(ruin.x - reach, ruin.z - reach);
	pile_.top.assign(size_t(pile_.side) * size_t(pile_.side), FAR_DOWN_M);
	pieces_.resize(cut->pieces.size());
	for (size_t i = 0; i < pieces_.size(); i++) {
		const RuinPiece &src = cut->pieces[i];
		Piece &p = pieces_[i];
		p.home = src.centre;
		p.p = src.centre;
		p.radius = src.radius;
		p.first_link = src.first_link;
		p.links = src.links;
		p.grounded = src.low <= std::max(ground, cut->floor) + RESTING_M; // on the ground, or on the floor its building stood on
		p.asleep = true;
		p.flat = std::min({ src.half.x, src.half.y, src.half.z });
		p.shape(src.half, 4.f * src.half.length_squared());
	}
	if (!standing) schedule();
	for (const Piece &p : pieces_) // what stays standing is in the way of what falls
		if (p.grounded && p.power <= 0.f) rest_on_pile(p);
}

// When each piece goes: what the blow reaches at once, thrown, then the break running on by the bonds from there.
void RuinSim::schedule() {
	const Vector3 blow(ruin_.blow_x, ruin_.blow_y, ruin_.blow_z);
	const float reach = scar_reach_m(ruin_.blow_force), power = THROW_BASE_MS + ruin_.blow_force * THROW_PER_FORCE_MS;
	stack_.clear();
	int nearest = 0;
	float closest = FARTHEST_M;
	for (size_t i = 0; i < pieces_.size(); i++) {
		Piece &p = pieces_[i];
		p.scheduled = p.loose; // what a scar already took passes the break on from where it was
		p.fall_at = clock_;
		if (p.loose) {
			stack_.push_back(int(i));
			continue;
		}
		const float d = p.home.distance_to(blow) - p.radius * 0.5f;
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
		pieces_[size_t(nearest)].scheduled = true;
		stack_.push_back(nearest);
	}
	for (size_t head = 0; head < stack_.size(); head++) { // a breadth first walk over the bonds
		const Piece &from = pieces_[size_t(stack_[head])];
		for (int l = 0; l < from.links; l++) {
			const int next = cut_->links[size_t(from.first_link + l)];
			Piece &p = pieces_[size_t(next)];
			if (p.scheduled) continue;
			p.scheduled = true;
			p.fall_at = from.fall_at + HOP_S * (HOP_LEAST + rng_.f() * HOP_SPAN);
			stack_.push_back(next);
		}
	}
	left_ = int(pieces_.size());
}

// The first standing piece at the point takes the blow (destruction.ts `strike`): the wall may stand back from where it was aimed.
void RuinSim::scar(const Scar &c) {
	const Vector3 aimed(c.x, c.y, c.z);
	const Piece *first = nullptr;
	float closest = FARTHEST_M;
	for (const Piece &p : pieces_) {
		const float d = p.home.distance_to(aimed);
		if (p.loose || p.grounded || d >= closest) continue;
		closest = d;
		first = &p;
	}
	if (!first) return;
	const Vector3 blow = first->home;
	ruin_.blow_x = blow.x;
	ruin_.blow_y = blow.y;
	ruin_.blow_z = blow.z;
	const float reach = scar_reach_m(c.force), power = THROW_BASE_MS + c.force * THROW_PER_FORCE_MS;
	for (Piece &p : pieces_) {
		const float d = p.home.distance_to(blow) - p.radius * 0.5f;
		if (p.loose || p.scheduled || d >= reach) continue;
		p.scheduled = true;
		p.fall_at = clock_;
		p.power = power * std::max(THROW_SHARE_LEAST, 1.f - d / reach);
	}
	support_in_ = 0;
	left_ = std::max(left_, 1);
}

void RuinSim::collapse(const Ruin &ruin, float heap) {
	ruin_ = ruin;
	heap_ = heap;
	schedule();
}

void RuinSim::let_go(Piece &p, const Vector3 &from, float power, std::vector<Note> *notes) {
	if (p.loose) return;
	p.loose = true;
	p.asleep = false;
	p.rest = 0;
	const Vector3 away = (p.p - from).normalized();
	p.v = away * (power * (THROW_LEAST + rng_.f() * THROW_SPAN));
	p.v.y += power * THROW_UP * rng_.f();
	const float spin = SPIN_BASE + power * SPIN_PER_POWER;
	p.w = Vector3(rng_.f() - 0.5f, rng_.f() - 0.5f, rng_.f() - 0.5f) * (TURN_RANGE * spin);
	if (notes) notes->push_back(Note{ p.p, p.radius, false });
}

// Every standing piece no longer joined to the ground through standing pieces falls.
void RuinSim::drop_unsupported(std::vector<Note> *notes) {
	held_.assign(pieces_.size(), 0);
	stack_.clear();
	for (size_t i = 0; i < pieces_.size(); i++)
		if (!pieces_[i].loose && pieces_[i].grounded) {
			held_[i] = 1;
			stack_.push_back(int(i));
		}
	while (!stack_.empty()) {
		const Piece &p = pieces_[size_t(stack_.back())];
		stack_.pop_back();
		for (int l = 0; l < p.links; l++) {
			const int next = cut_->links[size_t(p.first_link + l)];
			if (held_[size_t(next)] || pieces_[size_t(next)].loose) continue;
			held_[size_t(next)] = 1;
			stack_.push_back(next);
		}
	}
	for (size_t i = 0; i < pieces_.size(); i++)
		if (!pieces_[i].loose && !held_[i]) let_go(pieces_[i], pieces_[i].p + Vector3(0, 1, 0), BREAK_POWER_MS, notes);
}

// What lies under a point: the ground, the mound as Mounds draws it, and the pieces that came to rest there.
float RuinSim::top(float x, float z) const {
	const float dx = x - ruin_.x, dz = z - ruin_.z;
	const float u = (dx * cos_ - dz * sin_) / (ruin_.w * MOUND_SPREAD), v = (dx * sin_ + dz * cos_) / (ruin_.d * MOUND_SPREAD);
	const float mound = std::fabs(u) >= 0.5f || std::fabs(v) >= 0.5f ? ground_ : ground_ + heap_ * MOUND_FILL * Mounds::hill_at(u, v);
	return std::max(mound, pile_.at(x, z));
}

void RuinSim::rest_on_pile(const Piece &p) {
	Vector3 lo = p.p, hi = p.p;
	for (const Vector3 &c : p.corners) {
		const Vector3 w = p.p + p.q.xform(c);
		lo = lo.min(w);
		hi = hi.max(w);
	}
	// it is a piece of wall: lying, it adds a wall's depth over where it lies (its box, a corner's or a sloping roof's, is mostly air), however
	// it came to rest; a plate asleep on its edge is no column for the next to land on
	pile_.lay(lo, hi, std::min(hi.y, lo.y + std::min(2.f * p.flat, RUIN_WALL_DEPTH_M)));
}

void RuinSim::step(std::vector<Note> *notes) {
	clock_ += RIGID_STEP_S;
	support_in_ -= RIGID_STEP_S;
	for (Piece &p : pieces_) {
		const bool stays = p.grounded && p.power <= 0.f; // what stands on the ground stays, unless the blow itself threw it
		if (!p.loose && !stays && p.scheduled && clock_ >= p.fall_at) let_go(p, Vector3(ruin_.blow_x, ruin_.blow_y, ruin_.blow_z), p.power > 0.f ? p.power : BREAK_POWER_MS, notes);
	}
	if (support_in_ <= 0.f) {
		support_in_ = SUPPORT_EVERY_S;
		drop_unsupported(notes);
	}
	auto ground = [this](float x, float z) { return top(x, z); };
	left_ = 0;
	for (Piece &p : pieces_) {
		if (!p.loose) {
			left_ += p.grounded || !p.scheduled ? 0 : 1;
			continue;
		}
		if (p.asleep) continue;
		left_++;
		const float falling = p.v.y;
		const bool slept = rigid_step(p, ground, law_);
		if (p.landed < 0.f && p.touching) p.landed = 0.f; // its first touch: the clock runs from there, never in its flight
		if (p.landed >= 0.f) p.landed += RIGID_STEP_S;
		p.moved = true;
		if (notes && falling < -LAND_SPEED_MS && p.v.y > falling * 0.5f) notes->push_back(Note{ p.p, p.radius, true });
		if (!slept && p.landed <= LONGEST_ROCK_S) continue;
		p.asleep = true; // on a slope a plate rocks for good: the clock, from where it came down, ends it
		rest_on_pile(p);
	}
}

void RuinSim::settle() {
	for (int i = 0; i < SETTLE_MOST_STEPS && left_ > 0; i++) step(nullptr);
}

int RuinSim::flying() const {
	return int(std::count_if(pieces_.begin(), pieces_.end(), [](const Piece &p) { return p.loose && !p.asleep; }));
}

void RuinSim::clear_moved() {
	for (Piece &p : pieces_) p.moved = false;
}

void RuinSim::write(size_t i, float *rows) const {
	const Piece &p = pieces_[i];
	const Basis b(p.q);
	const Vector3 at = p.p - b.xform(p.home);
	constexpr int LINES = 3, FOUR = 4;
	for (int r = 0; r < LINES; r++) {
		rows[r * FOUR] = b.rows[r].x;
		rows[r * FOUR + 1] = b.rows[r].y;
		rows[r * FOUR + 2] = b.rows[r].z;
		rows[r * FOUR + 3] = at[r];
	}
}

uint32_t RuinSim::digest() const {
	uint32_t h = 0;
	for (const Piece &p : pieces_) {
		uint32_t bits[3];
		std::memcpy(bits, &p.p, sizeof(bits));
		h = hash_combine(hash_combine(hash_combine(h, bits[0]), bits[1]), bits[2]);
	}
	return h;
}

} // namespace city
