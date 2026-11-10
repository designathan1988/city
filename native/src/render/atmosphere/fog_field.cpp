// The fog's window (see fog_field.h).
#include "render/atmosphere/fog_field.h"

#include "sim/terrain/height_patch.h"

#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <algorithm>
#include <cmath>
#include <unordered_map>

using namespace godot;

namespace city {

namespace {
constexpr int FOG_RES = 512; // texels across the window
// LINGER_SHARE: fog_field.h; the view goes this share of the window from its middle before it follows
constexpr int CHANNELS = 4;
constexpr float SLAB_HEIGHTS = 3.f; // thin to under a twentieth three heights up
constexpr float BUCKET_M = 160.f;
constexpr float EMPTY_SLAB = 1e30f;
constexpr int64_t TEXTURE_BYTES = int64_t(FOG_RES) * FOG_RES * CHANNELS * int64_t(sizeof(uint16_t));
constexpr int SPAN_SLACK = 1; // texels added round a touched span: the float tests at a dab's rim and a sample's reach stay inside
constexpr size_t MOST_TERRAIN_SPANS = 64; // past this many the touched ground is kept as their bounding span
constexpr double FLOAT_MANTISSA = 16777216.0; // 2^24
constexpr float EXTENT_WINDOWS = 2.f; // a window's origin stands at most this many sides past the map's half

int64_t bucket_of(int i, int j) { return cell_key(i, j); }

// Every window computes a texel's centre as ox + (i + 0.5) t. That is the same float whatever the window when every multiple of
// t / 2 out to `extent` is exact (t / 2 = odd x 2^e with odd x steps under 2^24): a texel kept from an earlier window is then
// the one this window would compute. Otherwise each job remakes its whole window.
bool exact_lattice(float t, float extent) {
	int e = 0;
	const double m = std::frexp(double(t) * 0.5, &e) * FLOAT_MANTISSA;
	if (m <= 0 || m != std::floor(m)) return false;
	uint64_t odd = uint64_t(m);
	while ((odd & 1) == 0) odd >>= 1;
	const double steps = std::ceil(double(extent) / (double(t) * 0.5)) + 1;
	return double(odd) * steps < FLOAT_MANTISSA;
}

bool same(const FogDab &a, const FogDab &b) {
	return a.x == b.x && a.z == b.z && a.radius == b.radius && a.strength == b.strength && a.height == b.height && a.speed == b.speed && a.erase == b.erase;
}

// The texels whose centre lies within [lo, hi] along an axis of texel `t`, grown by the slack.
void texel_range(double lo, double hi, float t, int &from, int &to) {
	from = int(std::floor(lo / t - 0.5)) - SPAN_SLACK;
	to = int(std::ceil(hi / t - 0.5)) + SPAN_SLACK + 1;
}

FogField::Span dab_span(const FogDab &d, float t) {
	FogField::Span sp;
	texel_range(double(d.x) - d.radius, double(d.x) + d.radius, t, sp.i0, sp.i1);
	texel_range(double(d.z) - d.radius, double(d.z) + d.radius, t, sp.j0, sp.j1);
	return sp;
}

FogField::Span join(const FogField::Span &a, const FogField::Span &b) {
	return { std::min(a.i0, b.i0), std::min(a.j0, b.j0), std::max(a.i1, b.i1), std::max(a.j1, b.j1) };
}

// A window texel's place in the repeating texture: a world texel keeps it modulo the side, as the shader reads it (world / side).
size_t slot_of(float x, float z, float t) {
	int wi = int(std::floor(x / t)) % FOG_RES, wj = int(std::floor(z / t)) % FOG_RES;
	wi = (wi + FOG_RES) % FOG_RES;
	wj = (wj + FOG_RES) % FOG_RES;
	return size_t(wj) * FOG_RES + size_t(wi);
}
} // namespace

void FogField::reset() {
	slot_.reset(); // a job still running publishes nothing; the texels it kept go with its scratch
	seen_ = asked_ = ~uint64_t(0);
	terrain_.clear();
	any_ = false;
	view_revision_++;
}

void FogField::touch_terrain(const CityState &s, const std::vector<int> &rects) {
	const Heightfield &hf = s.terrain;
	const float t = s.catalog.nature.fog.texel_m;
	if (t <= 0) return;
	for (size_t i = 0; i + 3 < rects.size(); i += 4) {
		// a point samples the four grid nodes round it: a node moves the ground within one spacing of it
		Span sp;
		texel_range(double(hf.x_of(rects[i])) - hf.spacing, double(hf.x_of(rects[i + 2])) + hf.spacing, t, sp.i0, sp.i1);
		texel_range(double(hf.x_of(rects[i + 1])) - hf.spacing, double(hf.x_of(rects[i + 3])) + hf.spacing, t, sp.j0, sp.j1);
		terrain_.push_back(sp);
	}
	if (terrain_.size() > MOST_TERRAIN_SPANS) {
		Span all = terrain_[0];
		for (const Span &sp : terrain_) all = join(all, sp);
		terrain_.assign(1, all);
	}
}

void FogField::work(const CityState &s, float ox, float oz, uint64_t revision, std::vector<Span> terrain, uint64_t generation) {
	std::unique_ptr<Scratch> k = slot_.scratch(generation);
	FogDef def;
	Raster r;
	r.ox = ox;
	r.oz = oz;
	r.low = EMPTY_SLAB;
	r.high = -EMPTY_SLAB;
	float half = 0, water = 0;
	std::vector<Span> spans;
	std::vector<HeightPatch> patches;
	bool full = true;
	int kx = 0, kz = 0;
	{
		// What the job reads, copied while it holds the map: the dabs (when their revision moved), the ground under the texels
		// it will make.
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		def = s.catalog.nature.fog;
		const Heightfield &hf = s.terrain;
		half = hf.half_size();
		water = hf.water_level;
		r.side = def.texel_m * FOG_RES;
		kx = int(std::lround(ox / def.texel_m));
		kz = int(std::lround(oz / def.texel_m));
		Span changed{ 0, 0, 0, 0 };
		bool dabs_changed = false;
		if (k->revision != revision) {
			const std::vector<FogDab> &now = s.nature.fog;
			size_t first = 0;
			while (first < now.size() && first < k->dabs.size() && same(now[first], k->dabs[first])) first++;
			for (size_t i = first; i < std::max(now.size(), k->dabs.size()); i++) {
				// a texel no changed dab reaches folds the same dabs in the same order (fog_at skips a dab past its rim)
				const Span sp = dab_span(i < now.size() ? now[i] : k->dabs[i], def.texel_m);
				changed = dabs_changed ? join(changed, sp) : sp;
				dabs_changed = true;
				if (i < now.size() && i < k->dabs.size()) changed = join(changed, dab_span(k->dabs[i], def.texel_m));
			}
			k->dabs = now;
			k->revision = revision;
			k->buckets.clear();
			for (const FogDab &d : k->dabs)
				for (int j = int(std::floor((d.z - d.radius) / BUCKET_M)); j <= int(std::floor((d.z + d.radius) / BUCKET_M)); j++)
					for (int i = int(std::floor((d.x - d.radius) / BUCKET_M)); i <= int(std::floor((d.x + d.radius) / BUCKET_M)); i++) k->buckets[bucket_of(i, j)].push_back(&d);
		}
		if (!k->dabs.empty()) {
			const Span window{ kx, kz, kx + FOG_RES, kz + FOG_RES };
			full = !k->held || !exact_lattice(def.texel_m, half + r.side * EXTENT_WINDOWS) || std::abs(kx - k->kx) >= FOG_RES || std::abs(kz - k->kz) >= FOG_RES;
			if (full) {
				spans.push_back(window);
			} else {
				const int ox0 = k->kx, oz0 = k->kz;
				if (kx > ox0) spans.push_back({ ox0 + FOG_RES, kz, kx + FOG_RES, kz + FOG_RES });
				if (kx < ox0) spans.push_back({ kx, kz, ox0, kz + FOG_RES });
				if (kz > oz0) spans.push_back({ kx, oz0 + FOG_RES, kx + FOG_RES, kz + FOG_RES });
				if (kz < oz0) spans.push_back({ kx, kz, kx + FOG_RES, oz0 });
				for (const Span &sp : terrain) spans.push_back(sp);
				if (dabs_changed) spans.push_back(changed);
				for (Span &sp : spans) sp = { std::max(sp.i0, window.i0), std::max(sp.j0, window.j0), std::min(sp.i1, window.i1), std::min(sp.j1, window.j1) };
				spans.erase(std::remove_if(spans.begin(), spans.end(), [](const Span &sp) { return sp.i0 >= sp.i1 || sp.j0 >= sp.j1; }), spans.end());
			}
			for (const Span &sp : spans) {
				const float x0 = ox + (float(sp.i0 - kx) + 0.5f) * def.texel_m, x1 = ox + (float(sp.i1 - 1 - kx) + 0.5f) * def.texel_m;
				const float z0 = oz + (float(sp.j0 - kz) + 0.5f) * def.texel_m, z1 = oz + (float(sp.j1 - 1 - kz) + 0.5f) * def.texel_m;
				patches.push_back(hf.patch(x0, z0, x1, z1));
			}
		}
	}
	if (k->dabs.empty()) {
		k->held = false; // the next dabs are laid over a whole window
		slot_.publish(generation, std::move(r), std::move(k));
		return;
	}
	const size_t texels = size_t(FOG_RES) * FOG_RES;
	if (full) {
		k->texels.resize(int64_t(texels) * CHANNELS * int64_t(sizeof(uint16_t)));
		k->ground.assign(texels, 0.f);
		k->top.assign(texels, 0.f);
		k->on.assign(texels, 0);
	}
	uint16_t *t = reinterpret_cast<uint16_t *>(k->texels.ptrw());
	FogSample f;
	for (size_t n = 0; n < spans.size(); n++) {
		if (!slot_.current(generation)) return; // a reset came: nothing of this window is kept
		const Span &sp = spans[n];
		const HeightPatch &ground_patch = patches[n];
		for (int wj = sp.j0; wj < sp.j1; wj++)
			for (int wi = sp.i0; wi < sp.i1; wi++) {
				const int i = wi - kx, j = wj - kz;
				float x = ox + (i + 0.5f) * def.texel_m, z = oz + (j + 0.5f) * def.texel_m;
				auto it = k->buckets.find(bucket_of(int(std::floor(x / BUCKET_M)), int(std::floor(z / BUCKET_M))));
				f = FogSample{};
				if (it != k->buckets.end()) fog_at(it->second, x, z, def, f);
				const bool inside = x > -half && x < half && z > -half && z < half; // Heightfield::in_bounds
				float ground = inside ? ground_patch.height_at(x, z) : water;
				const size_t at = slot_of(x, z, def.texel_m);
				uint16_t *c = t + at * CHANNELS;
				bool on = f.density > def.min_density;
				c[0] = Math::make_half_float(on ? f.density : 0.f);
				c[1] = Math::make_half_float(ground);
				c[2] = Math::make_half_float(on ? f.height : 0.f);
				c[3] = Math::make_half_float(on ? f.speed : 0.f);
				k->on[at] = on ? 1 : 0;
				k->ground[at] = ground;
				k->top[at] = ground + f.height * SLAB_HEIGHTS;
			}
	}
	// The slab over the whole window, in the window's order (a scan from its first texel, as the whole raster ran).
	for (int j = 0; j < FOG_RES; j++)
		for (int i = 0; i < FOG_RES; i++) {
			const size_t at = slot_of(ox + (i + 0.5f) * def.texel_m, oz + (j + 0.5f) * def.texel_m, def.texel_m);
			if (!k->on[at]) continue;
			r.any = true;
			r.low = std::min(r.low, k->ground[at]);
			r.high = std::max(r.high, k->top[at]);
		}
	k->held = true;
	k->kx = kx;
	k->kz = kz;
	r.texels = k->texels; // shared until the next job writes (copy on write)
	slot_.publish(generation, std::move(r), std::move(k));
}

void FogField::sync(const CityState &s, float cam_x, float cam_z, FrameAdmission &f) {
	Raster done; // the texture's bytes are the frame ledger's: a result waits for a frame with room
	if (slot_.take(done, [&](const Raster &r) { return !r.any || f.upload.take(TEXTURE_BYTES); })) {
		seen_ = asked_;
		view_revision_++;
		any_ = done.any;
		ox_ = done.ox;
		oz_ = done.oz;
		side_ = done.side;
		low_ = done.low;
		high_ = done.high;
		if (any_) {
			Ref<Image> img = Image::create_from_data(FOG_RES, FOG_RES, false, Image::FORMAT_RGBAH, done.texels);
			if (texture_.is_valid()) texture_->update(img);
			else texture_ = ImageTexture::create_from_image(img);
		}
	}
	if (slot_.busy() || !s.jobs) return;
	const FogDef &def = s.catalog.nature.fog;
	float side = def.texel_m * FOG_RES, hold = side / LINGER_SHARE;
	bool moved = any_ && (std::fabs(cam_x - (ox_ + side * 0.5f)) > hold || std::fabs(cam_z - (oz_ + side * 0.5f)) > hold);
	uint64_t rev = s.nature.fog_revision;
	if (rev == seen_ && !moved) return;
	float ox = std::floor((cam_x - side * 0.5f) / def.texel_m) * def.texel_m, oz = std::floor((cam_z - side * 0.5f) / def.texel_m) * def.texel_m;
	asked_ = rev;
	std::vector<Span> terrain;
	terrain.swap(terrain_);
	const uint64_t generation = slot_.begin();
	s.jobs->submit(JobKey{ JobLane::FOG, 0 }, 0.f, [this, &s, ox, oz, rev, terrain = std::move(terrain), generation]() mutable { work(s, ox, oz, rev, std::move(terrain), generation); });
}

Dictionary FogField::view() const {
	Dictionary d;
	d["any"] = any_ && texture_.is_valid();
	if (any_ && texture_.is_valid()) {
		d["texture"] = texture_;
		d["slab"] = Vector2(low_, high_);
		d["window"] = Vector3(ox_, oz_, side_);
	}
	return d;
}

} // namespace city
