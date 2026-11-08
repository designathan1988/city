// What a lit window is, for every level of the buildings: the glass, the lamp of a room, who is awake behind it and the filtered
// window grid.
//
// A black body's colour at a temperature, linear and of luminance 1: M. Charity's sRGB table of the Planckian locus,
// checked against Kang et al. 2002 (within 4 of 255 on every channel). Homes mix warm 2700-3500 K lamps with cool white
// 6500 K; offices and shops neutral 4000 K, daylight 5000 K and some 3500 K. The mixes' means are worked out from these.
const vec3 LAMP_2700 = vec3(1.9871, 0.7884, 0.1894);
const vec3 LAMP_3000 = vec3(1.8194, 0.8304, 0.2675);
const vec3 LAMP_3500 = vec3(1.5988, 0.8826, 0.4000);
const vec3 LAMP_4000 = vec3(1.4387, 0.9173, 0.5269);
const vec3 LAMP_5000 = vec3(1.2315, 0.9554, 0.7601);
const vec3 LAMP_6500 = vec3(1.0405, 0.9857, 1.0221);
const vec3 LAMP_9000 = vec3(0.8745, 1.0013, 1.3569);
const vec3 LAMP_HOME_MEAN = vec3(1.6241, 0.8671, 0.4790);
const vec3 LAMP_OFFICE_MEAN = vec3(1.3902, 0.9254, 0.5895);

global uniform float rooms_awake; // the share of people awake at this hour (daylight.gd)
global uniform float rooms_in_use; // of those awake, the share whose room has the lamp on (daylight.json, daylight.gd)
global uniform float building_night_windows; // the Effects panel: 0 puts the lit rooms out

const vec3 PANE_ALBEDO = vec3(0.0075, 0.0096, 0.0115); // glass scatters next to nothing
const vec3 PANE_ALBEDO_DARK = vec3(0.0047, 0.0061, 0.0074); // a dark pane's

// The mixes the means above are worked out from, as the lots under which each lamp is picked: offices half 4000 K, 35 % 5000 K,
// the rest 3500 K; homes 36 % 2700 K, 24 % 3000 K, 10 % 3500 K, the rest 6500 K.
const float OFFICE_UNTIL_4000 = 0.5, OFFICE_UNTIL_5000 = 0.85;
const float HOME_UNTIL_2700 = 0.36, HOME_UNTIL_3000 = 0.6, HOME_UNTIL_3500 = 0.7;
// A room's independent picks from its lot: fract(lot * k), each k far from a simple ratio of the others.
const float LOT_LAMP = 17.31, LOT_LEVEL_A = 7.13, LOT_LEVEL_B = 2.17, LOT_LEVEL_B_SHIFT = 0.4, LOT_CUT = 37.0, LOT_FLICKER = 40.0;
const float LEVEL_LEAST = 0.3, LEVEL_SPAN = 0.85; // a lamp's level runs 0.3 to 1.15 over its two lots' product
// The far rooms' cell hash: each cell index offset to stay positive (more than a building's bays), times a prime.
const float CELL_SHIFT = 512.0;
const uint CELL_PRIME_X = 131u, CELL_PRIME_Y = 7177u;
// A television: about one cut every 1.7 s (0.6 a second), each cut's level from 0.45 to 0.9, a flicker of 8 % at 11 rad/s;
// its seed by Knuth's multiplicative hash (hash.gdshaderinc) and the lot's 13 bits.
const float TV_CUTS_HZ = 0.6, TV_CUT_LEAST = 0.45, TV_CUT_SPAN = 0.45, TV_FLICKER_FLOOR = 0.92, TV_FLICKER = 0.08, TV_FLICKER_RAD_S = 11.0;
const float LOT_BITS_TOP = 8191.0;
const float BOX_UNDER = 0.05; // a pulse narrower than this many periods a pixel is box-filtered (the tent's second integral loses precision)
const float LEAST_FOOT = 1e-4; // a grid footprint never under this

// The lamp of a room by its lot `room`: the colour takes a lot of its own from it.
vec3 room_lamp(float room, float office) {
	float lot = fract(room * LOT_LAMP);
	if (office > 0.5) return lot < OFFICE_UNTIL_4000 ? LAMP_4000 : lot < OFFICE_UNTIL_5000 ? LAMP_5000 : LAMP_3500;
	return lot < HOME_UNTIL_2700 ? LAMP_2700 : lot < HOME_UNTIL_3000 ? LAMP_3000 : lot < HOME_UNTIL_3500 ? LAMP_3500 : LAMP_6500;
}
vec3 room_lamp_mean(float office) { return office > 0.5 ? LAMP_OFFICE_MEAN : LAMP_HOME_MEAN; }
// How bright a room's lamp is, 0.3 to 1.15, by two lots of its own.
float room_level(float lot) { return LEVEL_LEAST + LEVEL_SPAN * fract(lot * LOT_LEVEL_A) * fract(lot * LOT_LEVEL_B + LOT_LEVEL_B_SHIFT); }

// A room of the far levels (building_far.gdshader, building_far_window.gdshaderinc), its cell `cell` of the bay grid: its own lot,
// lamp and whether it is lit while the cell covers a pixel or more, the share of rooms awake and the mean lamp as `sub` goes to 1.
uint room_cell(vec2 cell) { return uint(cell.x + CELL_SHIFT) * CELL_PRIME_X + uint(cell.y + CELL_SHIFT) * CELL_PRIME_Y; }
float far_room_lit(float lot, float sub) { return mix(step(1.0 - rooms_awake, lot), rooms_awake, sub) * building_night_windows; }
vec3 far_room_lamp(float lot, float office, float sub) { return mix(room_lamp(lot, office), room_lamp_mean(office), sub); }

// A television alone in the room: the cuts of a picture, about one every 1.7 s, and the small flicker over them.
vec3 tv_light(float clock, float lot) {
	float cut = h01(uint(clock * TV_CUTS_HZ + lot * LOT_CUT) * KNUTH_HASH + uint(lot * LOT_BITS_TOP));
	float shimmer = TV_FLICKER_FLOOR + TV_FLICKER * sin(clock * TV_FLICKER_RAD_S + lot * LOT_FLICKER);
	return LAMP_9000 * (TV_CUT_LEAST + TV_CUT_SPAN * cut) * shimmer;
}

// A pulse [a0, a1) repeated every 1, seen through a pixel w periods wide (Heckbert's summed-area idea applied to an analytic
// pattern, as in Quilez's "filterable procedurals"): the box over the first integral while a period holds many pixels, a tent
// over the second one below that. A tent's response falls as sinc squared, its first side lobe at -26.5 dB against a box's
// -13 dB, so the grid keeps reading at 2 pixels a period and does not shimmer. Taken on fract(x): the filtered pulse repeats
// with it, and the second integral of a far index would lose the float's precision.
float pulse_first(float x, float a0, float a1) { return floor(x) * (a1 - a0) + clamp(fract(x), a0, a1) - a0; }
float pulse_second(float x, float a0, float a1) {
	float l = a1 - a0, n = floor(x), f = x - n;
	float g = f < a0 ? 0.0 : f <= a1 ? 0.5 * (f - a0) * (f - a0) : 0.5 * l * l + l * (f - a1);
	return l * (0.5 * n * (n - 1.0) + n * f) + n * (0.5 * l * l + l * (1.0 - a1)) + g;
}
float pulse(float x, float w, float a0, float a1) {
	if (a1 <= a0) return 0.0;
	float mean = clamp(a1 - a0, 0.0, 1.0);
	float fade = smoothstep(0.5, 1.0, w);
	if (fade >= 1.0) return mean;
	float c = fract(x);
	float filtered = w < BOX_UNDER ? (pulse_first(c + 0.5 * w, a0, a1) - pulse_first(c - 0.5 * w, a0, a1)) / w
			: (pulse_second(c + w, a0, a1) - 2.0 * pulse_second(c, a0, a1) + pulse_second(c - w, a0, a1)) / (w * w);
	return mix(filtered, mean, fade);
}
// The footprint as the gradient's length per axis: fwidth's |dx| + |dy| is up to 1.41 times it in an oblique view.
vec2 grid_footprint(vec2 cell) {
	vec2 dx = dFdx(cell), dy = dFdy(cell);
	return max(sqrt(dx * dx + dy * dy), vec2(LEAST_FOOT));
}
