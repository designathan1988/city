// The mid level's window grid: `code` = opening width, sill, head (shares of the bay and the storey) and flags (shell.h). The
// wall, the frame, the glass and the roller shutter by the share of the pixel each covers; what a bay picks for itself fades to
// the share of bays that pick it as a bay drops under a few pixels, so the facades neither sparkle nor flatten. A pane's pick
// goes by its component (a PaneDark in the flags from WINDOW_DARK_SHIFT): a plain window one in four, with its shutters;
// a sash, a wide window, a balcony or a shopfront none; a loggia one in four; a ribbon one in five.
#include "res://render/shaders/contracts/buildings.gdshaderinc"
#include "res://render/shaders/bytes.gdshaderinc"
// The mid level's windows (materials.json building): a shutter's drop, from and over its pick; the share of a pane the far
// shutters cover and what of the pane's height they read; their tone.
uniform vec2 shutter_drop;
uniform vec2 shutter_far;
uniform vec3 shutter_tone;
const float LEAST_DET = 1e-9, FACE_NUDGE = 1e-3, LEAST_PANE_H = 1e-3; // guards: a determinant, a face's normal off the wall, a pane's height
// A bay's picks from its hash: its salt; dark panes, five of twenty or a fifth (FIFTH of them); a roller shutter part way down in
// four bays of twenty (picks 9 to 12); the lot from the ten bits over its byte; the floor's lot.
const uint PICK_SALT = 977u, PICK_OF = 20u, PICK_DARK = 5u, FIFTH_OF = 5u, SHUTTER_FROM = 9u, SHUTTER_TO = 13u, LOT_MASK = 1023u;
const float FIFTH = 0.2, LOT_SPAN = 1024.0, LOT_FLOOR = 11.3;
#include "res://render/shaders/fresnel.gdshaderinc"
const float FAR_ROOM_LEVEL = 0.68; // a far window's lamp: the mean of room_level (0.3 + 0.85 x two uniform lots)
const float FAR_FRAME_SHARE = 0.027; // the frame read round a pane: 8 cm of a 3 m bay or storey
const float FAR_REVEAL_SHADE = 0.42, REVEAL_SUN = 0.75; // a far reveal in the wall's shade, and what the sun adds on the faces that see it
const float SILL_SHARE = 0.018, SILL_DRIP = 0.05, SILL_LIGHT = 1.28, DRIP_DARK = 0.2; // the sill's line, and the shade it throws under it
const float RECESS_FROM = 0.1, RECESS_TO = 0.28; // of a cell per pixel: where the recess fades into the flat grid
const float GRAZING_LEAST = 0.08; // a ray along the wall stops here, or it would leave the bay

// How far a ray entering the wall along `into` moves over the grid in `depth` metres, in cells; `pu` and `pv` the metres of a cell.
vec2 grid_step(vec3 into, vec3 pu, vec3 pv, vec3 n, float depth) {
	return vec2(dot(into, pu) / dot(pu, pu), dot(into, pv) / dot(pv, pv)) * depth / max(-dot(into, n), GRAZING_LEAST);
}

// `depth`: how deep the pane stands, worked out here per pixel, the way a procedural facade is ray cast on its base polygon (Krecklau
// and Kobbelt): the pane is seen through the hole at that depth, the reveal round it takes the sun, the sill its line and its shade.
void far_window(vec2 uv, vec4 code, uint seed, vec3 n, vec3 V, float depth, inout vec3 albedo, out vec3 emit, out float glass) {
	emit = vec3(0.0);
	glass = 0.0;
	int flags = int(code.a * BYTE_TOP + 0.5);
	if ((flags & WINDOW_FLAG_BLANK) != 0) return;
	int picks = (flags >> WINDOW_DARK_SHIFT) & ((1 << WINDOW_DARK_BITS) - 1);
	vec2 foot = grid_footprint(uv), cell = floor(uv);
	float sub = smoothstep(0.25, 0.5, max(foot.x, foot.y));
	float a0 = 0.5 - 0.5 * code.r, a1 = 0.5 + 0.5 * code.r, f = FAR_FRAME_SHARE;
	float hole = pulse(uv.x, foot.x, a0, a1) * pulse(uv.y, foot.y, code.g, code.b);
	float recess = depth > 0.0 ? 1.0 - smoothstep(RECESS_FROM, RECESS_TO, max(foot.x, foot.y)) : 0.0;
	vec3 dpx = dFdx(v_world), dpy = dFdy(v_world);
	vec2 dux = dFdx(uv), duy = dFdy(uv);
	float det = dux.x * duy.y - dux.y * duy.x;
	det = abs(det) < LEAST_DET ? LEAST_DET : det;
	vec3 pu = (dpx * duy.y - dpy * dux.y) / det, pv = (dpy * dux.x - dpx * duy.x) / det;
	vec3 wall = albedo; // what a reveal shows, with no sill on it
	float line = recess * pulse(uv.x, foot.x, a0 - f, a1 + f);
	albedo *= (1.0 + (SILL_LIGHT - 1.0) * line * pulse(uv.y, foot.y, code.g - SILL_SHARE, code.g)) * (1.0 - DRIP_DARK * line * pulse(uv.y, foot.y, code.g - SILL_DRIP, code.g - SILL_SHARE));
	if (hole <= 0.0) return; // plain wall: most of a facade, with nothing below this to show (and no derivative left to take)
	vec2 back = uv + grid_step(V, pu, pv, n, depth) * recess;
	float across = pulse(back.x, foot.x, a0 + f, a1 - f);
	float pan = across * pulse(back.y, foot.y, code.g + f, code.b - f) * hole;
	vec2 at = fract(back);
	vec2 past = vec2(at.x < a0 ? -1.0 : at.x > a1 ? 1.0 : 0.0, at.y < code.g ? -1.0 : at.y > code.b ? 1.0 : 0.0);
	float reveal = recess * hole * (1.0 - pulse(back.x, foot.x, a0, a1) * pulse(back.y, foot.y, code.g, code.b));
	vec3 face = normalize(-past.x * normalize(pu) - past.y * normalize(pv) + n * FACE_NUDGE);
	vec3 wall_seen = wall * (FAR_REVEAL_SHADE + REVEAL_SUN * clamp(dot(face, sun_direction), 0.0, 1.0) * (1.0 - night));
	uint h = pcg(seed * PICK_SALT + room_cell(cell));
	float dark_near = picks == PANE_DARK_PICK ? (h % PICK_OF < PICK_DARK ? 1.0 : 0.0) : picks == PANE_DARK_QUARTER ? (h % 4u == 0u ? 1.0 : 0.0) : picks == PANE_DARK_FIFTH ? (h % FIFTH_OF == 0u ? 1.0 : 0.0) : 0.0;
	float dark = mix(dark_near, picks == PANE_DARK_FIFTH ? FIFTH : picks == PANE_DARK_PICK || picks == PANE_DARK_QUARTER ? 0.25 : 0.0, sub);
	float solid_near = 0.0;
	uint pick = h % PICK_OF;
	if (picks == PANE_DARK_PICK && pick >= SHUTTER_FROM && pick < SHUTTER_TO) { // a roller shutter part way down in four bays of twenty
		float down = (code.b - code.g) * (shutter_drop.x + float(pick - SHUTTER_FROM) * shutter_drop.y);
		solid_near = across * pulse(uv.y, foot.y, code.b - f - down, code.b - f);
	}
	float pane_h = max(code.b - code.g - 2.0 * f, LEAST_PANE_H);
	float solid = picks == PANE_DARK_PICK ? mix(solid_near, shutter_far.x * pan * min(shutter_far.y * (code.b - code.g) / pane_h, 1.0), sub) : 0.0;
	glass = max(pan - solid, 0.0);
	float frame = max(hole - pan, 0.0);
	vec3 trim = srgb_to_linear(v_trim.rgb), shutter = shutter_tone;
	float cosv = clamp(dot(-V, n), 0.0, 1.0);
	float F = max(schlick(cosv), glass_reflect);
	frame = max(frame - reveal, 0.0);
	albedo = albedo * (1.0 - frame - glass - solid - reveal) + wall_seen * reveal + trim * frame + mix(PANE_ALBEDO, PANE_ALBEDO_DARK, dark) * glass + shutter * solid;
	float lot = float((h >> BYTE_SHIFT) & LOT_MASK) / LOT_SPAN, office = (flags & WINDOW_FLAG_OFFICE) != 0 ? 1.0 : 0.0;
	float far_sub = smoothstep(0.5, 1.5, max(foot.x, foot.y));
	float lit = far_room_lit(lot, far_sub);
	vec3 lamp = far_room_lamp(lot, office, far_sub);
	float through = 1.0 - schlick(cosv); // what the glass lets out at this angle
	vec3 room = room_tone(V, n, office > 0.5 ? FLOOR_STONE : fract(lot * LOT_FLOOR) < 0.5 ? FLOOR_BOARD : FLOOR_TILE);
	vec3 seen = mix(pane_tone(lot, room), pane_mean(room_tone(V, n, office > 0.5 ? FLOOR_STONE : FLOOR_HOME)), far_sub);
	emit = ((lamp * lit * night * window_glow * FAR_ROOM_LEVEL + seen * room_daylight) * through * mix(1.0, dark_glass, dark) + glass_reflection(V, n, F, mix(GLASS_ENV, GLASS_ENV_DARK, dark))) * glass;
}
