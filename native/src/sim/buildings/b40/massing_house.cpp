// Houses made to measure (jobs/b40/massing.ts house): style, heights, roof kind and one of the forms wing, veranda,
// ell, twin and bungalow, with the veranda's floor, roof slab and posts as lot elements. 4.0 draw order.
#include "sim/buildings/b40/massing.h"

namespace city::b40 {

namespace {
constexpr float STOREY_MIN_H = 2.8f, GROUND_HI = 3.3f, STOREY_LO = 2.8f, STOREY_HI = 3.1f, HALF = 0.5f;
constexpr int PALETTES = 7;
constexpr float WIDE_LOT = 10.f, BUNGALOW_W = 12.f, BUNGALOW_D = 14.f;
constexpr float ELL_WING_SHARE = 0.45f, ELL_WING_LO = 3.5f, ELL_WING_HI = 5.f, ELL_STEP_SHARE = 0.35f, ELL_STEP_LO = 2.f;
constexpr float ELL_STEP_HI = 3.5f, MAIN_LO = 7.5f, MAIN_HI = 10.f, ELL_WING_DEPTH = 0.6f, ELL_SAME_STOREYS = 0.6f;
constexpr float VER_LO = 1.8f, VER_HI = 2.6f, VER_MAIN_LO = 8.f, VER_MAIN_HI = 11.f, VER_BACK_ROOM = 2.5f, VER_BACK = 0.6f;
constexpr float VER_BACK_MIN = 4.f, VER_BACK_LO = 0.4f, VER_BACK_HI = 0.6f, VER_INSET = 0.3f, VER_SHORT = 3.5f;
constexpr float VER_SHORT_SHARE = 0.6f, VER_FLOOR_H = 0.15f, VER_SLAB_DROP = 0.2f, VER_SLAB_H = 0.16f, VER_SLAB_PAD = 0.3f;
constexpr float VER_SLAB_TRIM = 0.1f, VER_SLAB_BACK = 0.05f, POST_EVERY = 2.6f, POST = 0.2f, POST_HALF = 0.15f;
constexpr float TWIN_LO = 0.42f, TWIN_HI = 0.58f, TWIN_BACK_LO = 0.8f, TWIN_BACK_HI = 2.f, TWIN_MAIN_LO = 8.f, TWIN_MAIN_HI = 10.f;
constexpr float BUNG_PITCH_LO = 20.f, BUNG_PITCH_HI = 28.f, WING_MIN_ROOM = 2.f, WING_MIN = 4.f, WING_LO = 0.45f, WING_HI = 0.7f;
constexpr float DOUBLE_DOOR = 0.3f;

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

void ell(Body &b, Xoshiro128 &r, float w, float d, int storeys, Roof roof) {
	float wing_w = snap(std::min(w * ELL_WING_SHARE, mixr(r, ELL_WING_LO, ELL_WING_HI)));
	float step = snap(std::min(d * ELL_STEP_SHARE, mixr(r, ELL_STEP_LO, ELL_STEP_HI)));
	bool left = r.f() < HALF;
	float main_d = snap(std::min(d - step, mixr(r, MAIN_LO, MAIN_HI)));
	add_block(b, { 0, step, w, main_d, 0, storeys, roof });
	int ws = r.f() < ELL_SAME_STOREYS ? storeys : 1;
	add_block(b, { left ? 0 : w - wing_w, 0, wing_w, step + snap(main_d * ELL_WING_DEPTH), 0, ws, roof == ROOF_FLAT ? ROOF_FLAT : ROOF_GABLE });
	roof_shape(b.volumes[0], r, 0);
	roof_shape(b.volumes[1], r, 1);
}

void veranda(const Data &data, Body &b, Xoshiro128 &r, float w, float d, int storeys, Roof roof, const Paint &trim) {
	float deep = snap(mixr(r, VER_LO, VER_HI));
	float main_d = snap(std::min(d - deep, mixr(r, VER_MAIN_LO, VER_MAIN_HI)));
	add_block(b, { 0, deep, w, main_d, 0, storeys, roof });
	if (d - deep - main_d >= VER_BACK_ROOM && r.f() < VER_BACK) {
		float ww = snap(std::max(std::min(w, VER_BACK_MIN), w * mixr(r, VER_BACK_LO, VER_BACK_HI)));
		add_block(b, { r.f() < HALF ? 0 : w - ww, deep + main_d, ww, snap(d - deep - main_d), 0, 1, ROOF_FLAT });
	}
	roof_shape(b.volumes[0], r);
	float s0 = VER_INSET, s1 = r.f() < HALF ? w - VER_INSET : std::max(VER_SHORT, w * VER_SHORT_SHARE);
	float vw = s1 - s0, cx = (s0 + s1) / 2;
	int floor_paint = b.add_paint(Paint{ FIN_CERAMIC, nudge(data.color("veranda_floor").rgb, r) });
	int trim_paint = b.add_paint(trim);
	element(b, EL_PAVEMENT, cx, deep / 2, vw, deep, 0, VER_FLOOR_H, floor_paint);
	element(b, EL_SLAB, cx, deep / 2 - VER_SLAB_BACK, vw + VER_SLAB_PAD, deep - VER_SLAB_TRIM, b.ground_h - VER_SLAB_DROP, VER_SLAB_H, trim_paint);
	int posts = std::max(2, int(std::round(vw / POST_EVERY)) + 1);
	for (int k = 0; k < posts; k++)
		element(b, EL_PILLAR, s0 + POST_HALF + (vw - 2 * POST_HALF) * float(k) / float(posts - 1), POST, POST, POST, 0, b.ground_h - VER_SLAB_DROP, trim_paint);
}

void twin(const Data &data, Body &b, Xoshiro128 &r, float w, float d, Roof roof, const Paint &wall) {
	float split = snap(w * mixr(r, TWIN_LO, TWIN_HI));
	float setback = snap(mixr(r, TWIN_BACK_LO, TWIN_BACK_HI));
	bool tall_left = r.f() < HALF;
	float main_d = snap(std::min(d - setback, mixr(r, TWIN_MAIN_LO, TWIN_MAIN_HI)));
	add_block(b, { 0, tall_left ? 0 : setback, split, main_d, 0, tall_left ? 2 : 1, roof });
	Roof other = house_roof(r, b.style);
	add_block(b, { split, tall_left ? setback : 0, w - split, main_d, 0, tall_left ? 1 : 2, other });
	roof_shape(b.volumes[0], r);
	roof_shape(b.volumes[1], r);
	b.volumes[1].wall = b.add_paint(other_skin(data, b.style, r, wall));
}

void wing(Body &b, Xoshiro128 &r, float w, float d, int storeys, Roof roof) {
	float main_d = snap(std::min(d, mixr(r, MAIN_LO, MAIN_HI)));
	add_block(b, { 0, 0, w, main_d, 0, storeys, roof });
	if (d - main_d >= WING_MIN_ROOM) {
		float ww = snap(std::max(std::min(w, WING_MIN), w * mixr(r, WING_LO, WING_HI)));
		bool left = r.f() < HALF;
		Roof rr = choose(r, { ROOF_FLAT, ROOF_SHED, roof });
		add_block(b, { left ? 0 : w - ww, main_d, ww, snap(d - main_d), 0, 1, rr });
	}
	roof_shape(b.volumes[0], r);
}
} // namespace

void house(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r) {
	b.style = uint8_t(style_for(data, KIND_HOUSE, r, env.character, env.era));
	Paint wall, trim;
	paints_of(data, b.style, r, wall, trim);
	Paint roof_paint = roof_of(r, b.style == STYLE_MODERN_HOUSE ? data.roof_tiles : clay_of(data));
	b.palette = r.irange(0, PALETTES);
	b.ground_h = mixr(r, STOREY_MIN_H, GROUND_HI);
	b.storey_h = mixr(r, STOREY_LO, STOREY_HI);
	look(b, wall, roof_paint, trim);
	b.ground = GROUND_HOUSE;
	int storeys = b.style == STYLE_SOBRADO ? 2 : r.f() < HALF ? 2 : 1;
	Roof roof = house_roof(r, b.style);
	float w = snap(env.W), d = snap(env.D);
	std::vector<Form> forms{ FORM_WING, FORM_VERANDA };
	if (env.W >= WIDE_LOT) { forms.push_back(FORM_ELL); forms.push_back(FORM_TWIN); }
	if (env.W >= BUNGALOW_W && env.D <= BUNGALOW_D && b.style == STYLE_BUNGALOW) { forms.push_back(FORM_BUNGALOW); forms.push_back(FORM_BUNGALOW); }
	int door_block = -1;
	switch (choose(r, forms)) {
		case FORM_ELL: ell(b, r, w, d, storeys, roof); door_block = 0; break;
		case FORM_VERANDA: veranda(data, b, r, w, d, storeys, roof, trim); break;
		case FORM_TWIN: twin(data, b, r, w, d, roof, wall); break;
		case FORM_BUNGALOW: {
			float main_d = snap(std::min(d, mixr(r, MAIN_LO, MAIN_HI)));
			add_block(b, { 0, 0, w, main_d, 0, 1, ROOF_HIP });
			roof_shape(b.volumes[0], r, 0);
			b.volumes[0].pitch = std::round(mixr(r, BUNG_PITCH_LO, BUNG_PITCH_HI));
			break;
		}
		default: wing(b, r, w, d, storeys, roof); break;
	}
	uint8_t door = r.f() < DOUBLE_DOOR ? BAY_DOUBLE_DOOR : BAY_DOOR;
	finish_body(data, b, env, r, door, door_block);
}

} // namespace city::b40
