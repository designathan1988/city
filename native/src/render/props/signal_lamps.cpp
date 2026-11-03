// Draws the lit lenses of the signal posts, see signal_lamps.h.
#include "render/props/signal_lamps.h"

#include "sim/traffic/traffic_state.h"

#include <algorithm>
#include <limits>

using namespace godot;

namespace city {

namespace {
constexpr uint8_t DARK = 0xff; // a post with no published state: both heads dim
constexpr int WALKER_SHIFT = 4, CAR_MASK = 0xf;

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
	reach_ = 0;
	for (int k = 0; k < int(Lens::COUNT); k++) {
		const MeshBuilder &m = kit.lens(Lens(k));
		for (int64_t i = 0; i < m.v.size(); i++) reach_ = std::max(reach_, m.v[i].length());
		lens_[k] = m.bake();
	}
}

void SignalLamps::reset() {
	for (MultiMeshDraw &d : draw_) d.free();
	chunks_.clear();
	shown_.clear();
	posts_changed_ = true;
	lit_ = 0;
}

void SignalLamps::set_chunk(int64_t key, std::vector<SignalHead> &&heads) {
	if (heads.empty()) {
		drop_chunk(key);
		return;
	}
	chunks_[key] = std::move(heads);
	posts_changed_ = true;
}

void SignalLamps::drop_chunk(int64_t key) {
	if (chunks_.erase(key)) posts_changed_ = true;
}

void SignalLamps::update(const PoseFrame &f) {
	std::vector<uint8_t> now;
	for (const auto &kv : chunks_)
		for (const SignalHead &h : kv.second) {
			const SignalRecord *r = h.ds >= 0 ? f.signal(uint32_t(h.ds)) : nullptr;
			const Lens walker = r && r->walk ? Lens::WALK : Lens::STOP;
			now.push_back(r ? uint8_t(int(car_lens(r->light)) | int(walker) << WALKER_SHIFT) : DARK);
		}
	if (!posts_changed_ && now == shown_) return;
	posts_changed_ = false;
	shown_ = now;
	PackedFloat32Array buf[int(Lens::COUNT)];
	const float big = std::numeric_limits<float>::max();
	Vector3 lo(big, big, big), hi(-big, -big, -big);
	size_t i = 0;
	lit_ = 0;
	for (const auto &kv : chunks_)
		for (const SignalHead &h : kv.second) {
			const uint8_t code = now[i++];
			if (code == DARK) continue;
			const Vector3 x = h.basis.get_column(0), y = h.basis.get_column(1), z = h.basis.get_column(2);
			for (int k : { code & CAR_MASK, code >> WALKER_SHIFT }) {
				mm_push(buf[k], x, y, z, h.origin, Color(1, 1, 1, 1), 1, 1, 1, 1);
				lit_++;
			}
			lo = Vector3(std::min(lo.x, h.origin.x), std::min(lo.y, h.origin.y), std::min(lo.z, h.origin.z));
			hi = Vector3(std::max(hi.x, h.origin.x), std::max(hi.y, h.origin.y), std::max(hi.z, h.origin.z));
		}
	// the posts' feet, grown by the lenses' reach from them
	const Vector3 pad(reach_, reach_, reach_);
	const AABB box = lit_ > 0 ? AABB(lo - pad, hi - lo + pad * 2) : AABB();
	for (int k = 0; k < int(Lens::COUNT); k++) draw_[k].set(scenario_, lens_[k].rid, material_, buf[k], box, false);
}

} // namespace city
