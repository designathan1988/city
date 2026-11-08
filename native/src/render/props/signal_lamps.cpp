// Draws the lenses of the signal posts, see signal_lamps.h.
#include "render/props/signal_lamps.h"

#include "sim/traffic/traffic_state.h"

#include <algorithm>
#include <limits>

using namespace godot;

namespace city {

namespace {
constexpr uint8_t DARK = 0xff; // a post with no published state: every lens unlit
constexpr int WALKER_SHIFT = 4, CAR_MASK = 0xf; // a post's state byte: the walkers' light over the cars' low four bits
constexpr float NONE_LIT = -1.f; // INSTANCE_CUSTOM of a head with neither of its lenses lit (props.gdshader)
constexpr int LIT_PER_POST = 2; // a published post: its car lens and its walkers' lens

Lens car_lens(uint8_t light) {
	switch (Light(light)) {
		case Light::GREEN: return Lens::GREEN;
		case Light::AMBER: return Lens::AMBER;
		default: return Lens::RED;
	}
}
} // namespace

void SignalLamps::init(const RID &scenario, const RID &material, const PropKit &kit) {
	scenario_ = scenario;
	material_ = material;
	const MeshBuilder &m = kit.lenses();
	reach_ = 0;
	for (int64_t i = 0; i < m.v.size(); i++) reach_ = std::max(reach_, m.v[i].length());
	lenses_ = m.bake();
}

void SignalLamps::reset() {
	for (auto &kv : chunks_) kv.second.draw.free();
	chunks_.clear();
	signals_.clear();
	copied_ = false;
	lit_ = 0;
}

uint8_t SignalLamps::showing(const SignalHead &h) const {
	const SignalRecord *r = h.ds >= 0 ? find_signal(signals_, uint32_t(h.ds)) : nullptr;
	if (!r) return DARK;
	const Lens walker = r->walk ? Lens::WALK : Lens::STOP;
	return uint8_t(int(car_lens(r->light)) | int(walker) << WALKER_SHIFT);
}

void SignalLamps::write(Chunk &c) const {
	PackedFloat32Array buf;
	c.lit = 0;
	for (size_t i = 0; i < c.heads.size(); i++) {
		const SignalHead &h = c.heads[i];
		const uint8_t code = c.shown[i];
		const bool on = code != DARK;
		const Vector3 x = h.basis.get_column(0), y = h.basis.get_column(1), z = h.basis.get_column(2);
		mm_push(buf, x, y, z, h.origin, Color(1, 1, 1, 1), on ? float(code & CAR_MASK) : NONE_LIT,
				on ? float(code >> WALKER_SHIFT) : NONE_LIT, 0, 0);
		if (on) c.lit += LIT_PER_POST;
	}
	c.draw.set(scenario_, lenses_.rid, material_, buf, c.box, false, LAYER_SIGNALS);
}

void SignalLamps::set_chunk(int64_t key, std::vector<SignalHead> &&heads) {
	if (heads.empty()) {
		drop_chunk(key);
		return;
	}
	Chunk &c = chunks_[key];
	lit_ -= c.lit;
	c.heads = std::move(heads);
	c.shown.resize(c.heads.size());
	const float big = std::numeric_limits<float>::max();
	Vector3 lo(big, big, big), hi(-big, -big, -big);
	for (size_t i = 0; i < c.heads.size(); i++) {
		const Vector3 &o = c.heads[i].origin;
		c.shown[i] = showing(c.heads[i]);
		lo = Vector3(std::min(lo.x, o.x), std::min(lo.y, o.y), std::min(lo.z, o.z));
		hi = Vector3(std::max(hi.x, o.x), std::max(hi.y, o.y), std::max(hi.z, o.z));
	}
	const Vector3 pad(reach_, reach_, reach_);
	c.box = AABB(lo - pad, hi - lo + pad * 2);
	write(c);
	lit_ += c.lit;
}

void SignalLamps::drop_chunk(int64_t key) {
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return;
	lit_ -= it->second.lit;
	it->second.draw.free();
	chunks_.erase(it);
}

void SignalLamps::update(const PoseFrame &f, bool fresh) {
	if (!fresh && copied_) return; // the same publication: every post already shows it
	copied_ = true;
	signals_.assign(f.signals.begin(), f.signals.end());
	for (auto &kv : chunks_) {
		Chunk &c = kv.second;
		bool changed = false;
		for (size_t i = 0; i < c.heads.size(); i++) {
			const uint8_t code = showing(c.heads[i]);
			changed = changed || code != c.shown[i];
			c.shown[i] = code;
		}
		if (!changed) continue;
		lit_ -= c.lit;
		write(c);
		lit_ += c.lit;
	}
}

} // namespace city
