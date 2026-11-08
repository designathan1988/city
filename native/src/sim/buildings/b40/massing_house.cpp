// Houses made to measure: style, heights, roof kind and one of the forms wing, veranda, ell, twin and bungalow, with
// the veranda's floor, roof slab and posts as lot elements.
#include "sim/buildings/b40/massing.h"

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f; // the heights and the forms' sizes are buildings.json shapes.house, with their sources

enum Form { FORM_WING, FORM_VERANDA, FORM_ELL, FORM_TWIN, FORM_BUNGALOW };

Roof house_roof(Xoshiro128 &r, int style) {
	if (style == STYLE_COLONIAL_HOUSE) return choose(r, { ROOF_FLAT, ROOF_FLAT, ROOF_GABLE, ROOF_HIP });
	if (style == STYLE_MODERN_HOUSE) return choose(r, { ROOF_FLAT, ROOF_FLAT, ROOF_FLAT, ROOF_SHED });
	if (style == STYLE_SOBRADO) return choose(r, { ROOF_GABLE, ROOF_HIP, ROOF_FLAT });
	return choose(r, { ROOF_GABLE, ROOF_HIP, ROOF_HIP });
}

void element(Body &b, Element k, float x, float y, float w, float d, float z, float h, int paint) {
	LotElement e;
	e.kind = k; e.x = x; e.y = y; e.w = w; e.d = d; e.z = z; e.h = h; e.paint = paint;
	b.elements.push_back(e);
}

void ell(const Data &data, Body &b, Xoshiro128 &r, float w, float d, int storeys, Roof roof) {
	const HouseShape &K = data.shapes.house;
	float wing_w = snap(std::min(w * K.ell_wing_share, mixr(r, K.ell_wing_lo, K.ell_wing_hi)));
	float step = snap(std::min(d * K.ell_step_share, mixr(r, K.ell_step_lo, K.ell_step_hi)));
	bool left = r.f() < HALF;
	float main_d = snap(std::min(d - step, mixr(r, K.main_lo, K.main_hi)));
	add_block(b, { 0, step, w, main_d, 0, storeys, roof });
	int ws = r.f() < K.ell_same_storeys ? storeys : 1;
	add_block(b, { left ? 0 : w - wing_w, 0, wing_w, step + snap(main_d * K.ell_wing_depth), 0, ws, roof == ROOF_FLAT ? ROOF_FLAT : ROOF_GABLE });
	roof_shape(data, b.volumes[0], r, 0);
	roof_shape(data, b.volumes[1], r, 1);
}

void veranda(const Data &data, Body &b, Xoshiro128 &r, float w, float d, int storeys, Roof roof, const Paint &trim) {
	const HouseShape &K = data.shapes.house;
	float deep = snap(mixr(r, K.ver_lo, K.ver_hi));
	float main_d = snap(std::min(d - deep, mixr(r, K.ver_main_lo, K.ver_main_hi)));
	add_block(b, { 0, deep, w, main_d, 0, storeys, roof });
	if (d - deep - main_d >= K.ver_back_room && r.f() < K.ver_back) {
		float ww = snap(std::max(std::min(w, K.ver_back_min), w * mixr(r, K.ver_back_lo, K.ver_back_hi)));
		add_block(b, { r.f() < HALF ? 0 : w - ww, deep + main_d, ww, snap(d - deep - main_d), 0, 1, ROOF_FLAT });
	}
	roof_shape(data, b.volumes[0], r);
	float s0 = K.ver_inset, s1 = r.f() < HALF ? w - K.ver_inset : std::max(K.ver_short, w * K.ver_short_share);
	float vw = s1 - s0, cx = (s0 + s1) / 2;
	int floor_paint = b.add_paint(Paint{ FIN_CERAMIC, nudge(data.shapes.paint, data.color("veranda_floor").rgb, r) });
	int trim_paint = b.add_paint(trim);
	element(b, EL_PAVEMENT, cx, deep / 2, vw, deep, 0, K.ver_floor_h, floor_paint);
	element(b, EL_SLAB, cx, deep / 2 - K.ver_slab_back, vw + K.ver_slab_pad, deep - K.ver_slab_trim, b.ground_h - K.ver_slab_drop, K.ver_slab_h, trim_paint);
	int posts = std::max(2, int(std::round(vw / K.post_every)) + 1);
	for (int k = 0; k < posts; k++)
		element(b, EL_PILLAR, s0 + K.post_half + (vw - 2 * K.post_half) * float(k) / float(posts - 1), K.post, K.post, K.post, 0, b.ground_h - K.ver_slab_drop, trim_paint);
}

void twin(const Data &data, Body &b, Xoshiro128 &r, float w, float d, Roof roof, const Paint &wall) {
	const HouseShape &K = data.shapes.house;
	float split = snap(w * mixr(r, K.twin_lo, K.twin_hi));
	float setback = snap(mixr(r, K.twin_back_lo, K.twin_back_hi));
	bool tall_left = r.f() < HALF;
	float main_d = snap(std::min(d - setback, mixr(r, K.twin_main_lo, K.twin_main_hi)));
	add_block(b, { 0, tall_left ? 0 : setback, split, main_d, 0, tall_left ? 2 : 1, roof });
	Roof other = house_roof(r, b.style);
	add_block(b, { split, tall_left ? setback : 0, w - split, main_d, 0, tall_left ? 1 : 2, other });
	roof_shape(data, b.volumes[0], r);
	roof_shape(data, b.volumes[1], r);
	b.volumes[1].wall = b.add_paint(other_skin(data, b.style, r, wall));
}

void wing(const Data &data, Body &b, Xoshiro128 &r, float w, float d, int storeys, Roof roof) {
	const HouseShape &K = data.shapes.house;
	float main_d = snap(std::min(d, mixr(r, K.main_lo, K.main_hi)));
	add_block(b, { 0, 0, w, main_d, 0, storeys, roof });
	if (d - main_d >= K.wing_min_room) {
		float ww = snap(std::max(std::min(w, K.wing_min), w * mixr(r, K.wing_lo, K.wing_hi)));
		bool left = r.f() < HALF;
		Roof rr = choose(r, { ROOF_FLAT, ROOF_SHED, roof });
		add_block(b, { left ? 0 : w - ww, main_d, ww, snap(d - main_d), 0, 1, rr });
	}
	roof_shape(data, b.volumes[0], r);
}
} // namespace

void house(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r) {
	const HouseShape &K = data.shapes.house;
	b.style = uint8_t(style_for(data, KIND_HOUSE, r, env.character, env.era));
	Paint wall, trim;
	paints_of(data, b.style, r, wall, trim);
	Paint roof_paint = roof_of(data, r, b.style == STYLE_MODERN_HOUSE ? data.roof_tiles : clay_of(data));
	b.palette = r.irange(0, data.last_palette());
	b.ground_h = mixr(r, K.storey_min_h, K.ground_hi);
	b.storey_h = mixr(r, K.storey_lo, K.storey_hi);
	look(b, wall, roof_paint, trim);
	b.ground = GROUND_HOUSE;
	int storeys = b.style == STYLE_SOBRADO ? 2 : r.f() < HALF ? 2 : 1;
	Roof roof = house_roof(r, b.style);
	float w = snap(env.W), d = snap(env.D);
	std::vector<Form> forms{ FORM_WING, FORM_VERANDA };
	if (env.W >= K.wide_lot) { forms.push_back(FORM_ELL); forms.push_back(FORM_TWIN); }
	if (env.W >= K.bungalow_w && env.D <= K.bungalow_d && b.style == STYLE_BUNGALOW) { forms.push_back(FORM_BUNGALOW); forms.push_back(FORM_BUNGALOW); }
	int door_block = -1;
	switch (choose(r, forms)) {
		case FORM_ELL: ell(data, b, r, w, d, storeys, roof); door_block = 0; break;
		case FORM_VERANDA: veranda(data, b, r, w, d, storeys, roof, trim); break;
		case FORM_TWIN: twin(data, b, r, w, d, roof, wall); break;
		case FORM_BUNGALOW: {
			float main_d = snap(std::min(d, mixr(r, K.main_lo, K.main_hi)));
			add_block(b, { 0, 0, w, main_d, 0, 1, ROOF_HIP });
			roof_shape(data, b.volumes[0], r, 0);
			b.volumes[0].pitch = std::round(mixr(r, K.bung_pitch_lo, K.bung_pitch_hi));
			break;
		}
		default: wing(data, b, r, w, d, storeys, roof); break;
	}
	uint8_t door = r.f() < K.double_door ? BAY_DOUBLE_DOOR : BAY_DOOR;
	finish_body(data, b, env, r, door, door_block);
}

} // namespace city::b40
