// What a cooked clip says on one body (people_gait_clips.h).
#include "render/agents/people_gait_clips.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float LEAST = 1e-6f; // a duration, a pace or a span under this is none
} // namespace

int GaitClipFacts::female(int body) const { return asset_->variations[size_t(body)].female ? 1 : 0; }

float GaitClipFacts::seconds(int clip, int body) const { return asset_->clips[size_t(clip)].seconds[female(body)]; }

float GaitClipFacts::stride(int clip, int body, float size) const { return asset_->variations[size_t(body)].fits[size_t(clip)].stride * size; }

float GaitClipFacts::natural_pace(int clip, int body, float size) const { return stride(clip, body, size) / std::max(seconds(clip, body), LEAST); }

float GaitClipFacts::clock_phase(int clip, int body, double clock, uint32_t seed) const {
	const double turns = clock / double(std::max(seconds(clip, body), LEAST)) + double(seed) * double(spread_);
	return float(turns - std::floor(turns));
}

int GaitClipFacts::last_frame(int clip) const {
	const PersonClip &c = asset_->clips[size_t(clip)];
	return c.loop ? c.frames : c.frames - 1;
}

float GaitClipFacts::curve_at(int clip, int body, float fraction) const {
	const int last = last_frame(clip);
	const float x = clampf(fraction, 0.f, 1.f) * float(last);
	const int k = std::min(last - 1, int(std::floor(x)));
	if (k < 0) return asset_->curve(body, clip, 0);
	const float a = asset_->curve(body, clip, k), b = asset_->curve(body, clip, k + 1);
	return a + (b - a) * (x - float(k));
}

float GaitClipFacts::fraction_at(int clip, int body, float value) const {
	const int last = last_frame(clip);
	if (last <= 0 || value <= asset_->curve(body, clip, 0)) return 0.f;
	if (value >= asset_->curve(body, clip, last)) return 1.f;
	int lo = 0, hi = last;
	while (hi - lo > 1) {
		const int mid = (lo + hi) / 2;
		if (asset_->curve(body, clip, mid) < value) lo = mid;
		else hi = mid;
	}
	const float a = asset_->curve(body, clip, lo), span = asset_->curve(body, clip, hi) - a;
	return (float(lo) + (span > LEAST ? (value - a) / span : 0.f)) / float(last);
}

float GaitClipFacts::clip_speed(int clip, int body, int frame, float size) const {
	const int last = std::max(last_frame(clip), 1);
	return (asset_->curve(body, clip, frame) - asset_->curve(body, clip, frame - 1)) * size / (seconds(clip, body) / float(last));
}

float GaitClipFacts::stop_entry(int stop, int body, float speed, float size) const {
	const int last = last_frame(stop);
	float peak = 0;
	int peak_at = 0;
	for (int i = 1; i <= last; i++) {
		const float v = clip_speed(stop, body, i, size);
		if (v > peak) {
			peak = v;
			peak_at = i;
		}
	}
	for (int i = std::max(1, peak_at); i <= last; i++)
		if (clip_speed(stop, body, i, size) <= speed) return float(i - 1) / float(last);
	return 1.f;
}

float GaitClipFacts::start_entry(int start, int body, float speed, float size) const {
	const int last = last_frame(start);
	for (int i = 1; i <= last; i++)
		if (clip_speed(start, body, i, size) >= speed) return float(i - 1) / float(last);
	return 0.f;
}

} // namespace city
