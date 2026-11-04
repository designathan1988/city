// The mid level's window grid: `code` = opening width, sill, head (shares of the bay and the storey) and flags (shell.h). The
// wall, the frame, the glass and the roller shutter by the share of the pixel each covers; what a bay picks for itself fades to
// the share of bays that pick it as a bay drops under a few pixels, so the facades neither sparkle nor flatten. A pane's pick
// goes by its component (flags above WINDOW_DARK_SHIFT, facade_bay.h PaneDark): a plain window one in four, with its shutters;
// a sash, a wide window, a balcony or a shopfront none; a loggia one in four; a ribbon one in five.
const int DARK_SHIFT = 4;
const int DARK_PICK = 0;
const int DARK_QUARTER = 2;
const int DARK_FIFTH = 3;
const float FAR_ROOM_LEVEL = 0.68; // a far window's lamp: the mean of pane_lamp's level (0.3 + 0.85 x two uniform lots)
const float FAR_FRAME_SHARE = 0.027; // the frame read round a pane: 8 cm of a 3 m bay or storey
void far_window(vec2 uv, vec4 code, uint seed, vec3 n, vec3 V, inout vec3 albedo, out vec3 emit, out float glass) {
	emit = vec3(0.0);
	glass = 0.0;
	int flags = int(code.a * 255.0 + 0.5);
	if ((flags & 4) != 0) return;
	int picks = (flags >> DARK_SHIFT) & 3;
	vec2 foot = grid_footprint(uv), cell = floor(uv);
	float sub = smoothstep(0.25, 0.5, max(foot.x, foot.y));
	float a0 = 0.5 - 0.5 * code.r, a1 = 0.5 + 0.5 * code.r, f = FAR_FRAME_SHARE;
	float hole = pulse(uv.x, foot.x, a0, a1) * pulse(uv.y, foot.y, code.g, code.b);
	float across = pulse(uv.x, foot.x, a0 + f, a1 - f);
	float pan = across * pulse(uv.y, foot.y, code.g + f, code.b - f);
	uint h = pcg(seed * 977u + uint(cell.x + 512.0) * 131u + uint(cell.y + 512.0) * 7177u);
	float dark_near = picks == DARK_PICK ? (h % 20u < 5u ? 1.0 : 0.0) : picks == DARK_QUARTER ? (h % 4u == 0u ? 1.0 : 0.0) : picks == DARK_FIFTH ? (h % 5u == 0u ? 1.0 : 0.0) : 0.0;
	float dark = mix(dark_near, picks == DARK_FIFTH ? 0.2 : picks == DARK_PICK || picks == DARK_QUARTER ? 0.25 : 0.0, sub);
	float solid_near = 0.0;
	uint pick = h % 20u;
	if (picks == DARK_PICK && pick >= 9u && pick < 13u) { // a roller shutter part way down in four bays of twenty
		float down = (code.b - code.g) * (0.25 + float(pick - 9u) * 0.15);
		solid_near = across * pulse(uv.y, foot.y, code.b - f - down, code.b - f);
	}
	float pane_h = max(code.b - code.g - 2.0 * f, 1e-3);
	float solid = picks == DARK_PICK ? mix(solid_near, 0.2 * pan * min(0.475 * (code.b - code.g) / pane_h, 1.0), sub) : 0.0;
	glass = max(pan - solid, 0.0);
	float frame = max(hole - pan, 0.0);
	vec3 trim = to_linear(v_trim.rgb), shutter = vec3(0.32, 0.36, 0.37);
	float cosv = clamp(dot(-V, n), 0.0, 1.0);
	float F = max(0.04 + 0.96 * pow(1.0 - cosv, 5.0), glass_reflect);
	albedo = albedo * (1.0 - frame - glass - solid) + trim * frame + mix(vec3(0.0075, 0.0096, 0.0115), vec3(0.0047, 0.0061, 0.0074), dark) * glass + shutter * solid;
	float lot = float((h >> 8u) & 1023u) / 1024.0, office = (flags & 8) != 0 ? 1.0 : 0.0;
	float far_sub = smoothstep(0.5, 1.5, max(foot.x, foot.y));
	float lit = mix(step(1.0 - rooms_awake, lot), rooms_awake, far_sub);
	vec3 lamp = mix(room_lamp(fract(lot * 17.31), office), room_lamp_mean(office), far_sub);
	float through = 1.0 - (0.04 + 0.96 * pow(1.0 - cosv, 5.0)); // what the glass lets out at this angle (Schlick, F0 4 %)
	emit = ((lamp * lit * night * window_glow * FAR_ROOM_LEVEL + PANE_MEAN_SEEN * room_daylight) * through * mix(1.0, 0.64, dark) + glass_reflection(V, n, F)) * glass;
}
