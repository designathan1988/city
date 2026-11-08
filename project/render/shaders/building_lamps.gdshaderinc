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
global uniform float offices_lit; // the share of an office's lights on at this hour (daylight.json office_lit_by_hour)
const float ROOMS_IN_USE = 0.64; // of the people awake at home, the rooms with the lamp on (the Roadcraft 4.0's vRoomLot)

// The share of a building's rooms lit now: a home's by who is awake and the rooms in use, an office's by its lighting schedule.
float rooms_lit(float office) {
	return mix(rooms_awake * ROOMS_IN_USE, offices_lit, office);
}
global uniform float building_night_windows; // the Effects panel: 0 puts the lit rooms out
global uniform float lux; // the radiance of a white floor under 1 lx (render/daylight.gd, tuning.json render.light_unit_lx)
// A lit room's working plane, home and office (daylight.json rooms), as the radiance of a white floor under it: its surfaces show
// their albedo times it, as by day they show it times the daylight that comes in.
global uniform vec2 room_lux;
const float LAMP_PI = 3.14159265; // a luminance L in cd/m^2 is the radiance L pi lux

float room_light_level(float office) {
	return mix(room_lux.x, room_lux.y, office);
}

// An outside lantern's glass as a light: its colour (luminance about 1) times its luminance in cd/m^2.
vec3 lantern_light(vec3 colour, float cd_m2) {
	return colour * cd_m2 * LAMP_PI * lux;
}

const float GLASS_ROUGHNESS = 0.0; // float glass (physicallybased.info, Glass: roughness 0): it mirrors, through the engine's reflections
// A glazed opening's reflectance at normal incidence (materials.json glass): double glazing in a home, a solar-control coating in an
// office. Glass scatters next to nothing, so a pane is drawn as the mirror it is, METALLIC 1 with this F0 (the engine's Schlick takes it
// to 1 at grazing), the room through it its emission, times what the glazing lets through at that angle.
uniform float glazing_reflect = 0.15;
uniform float coated_reflect = 0.25;
float glazing_f0(float office) {
	return mix(glazing_reflect, coated_reflect, office);
}
float glazing_through(float f0, float cosv) {
	return 1.0 - (f0 + (1.0 - f0) * pow(1.0 - cosv, 5.0));
}

// The lamp of a room by its lot `room`: the colour takes a lot of its own from it.
vec3 room_lamp(float room, float office) {
	float lot = fract(room * 17.31);
	if (office > 0.5) return lot < 0.5 ? LAMP_4000 : lot < 0.85 ? LAMP_5000 : LAMP_3500;
	return lot < 0.36 ? LAMP_2700 : lot < 0.6 ? LAMP_3000 : lot < 0.7 ? LAMP_3500 : LAMP_6500;
}
vec3 room_lamp_mean(float office) { return office > 0.5 ? LAMP_OFFICE_MEAN : LAMP_HOME_MEAN; }
// How bright a room's lamp is, 0.3 to 1.15, by two lots of its own.
float room_level(float lot) { return 0.3 + 0.85 * fract(lot * 7.13) * fract(lot * 2.17 + 0.4); }

// A room of the far levels (building_far.gdshader, building_far_window.gdshaderinc), its cell `cell` of the bay grid: its own lot,
// lamp and whether it is lit while the cell covers a pixel or more, the share of rooms lit and the mean lamp as `sub` goes to 1.
uint room_cell(vec2 cell) { return uint(cell.x + 512.0) * 131u + uint(cell.y + 512.0) * 7177u; }
float far_room_lit(float lot, float sub, float office) {
	float share = rooms_lit(office);
	return mix(step(1.0 - share, lot), share, sub) * building_night_windows;
}
vec3 far_room_lamp(float lot, float office, float sub) { return mix(room_lamp(lot, office), room_lamp_mean(office), sub); }

// A television alone in the room: the cuts of a picture, about one every 1.7 s, and the small flicker over them.
vec3 tv_light(float clock, float lot) {
	float cut = h01(uint(clock * 0.6 + lot * 37.0) * 2654435761u + uint(lot * 8191.0));
	float shimmer = 0.92 + 0.08 * sin(clock * 11.0 + lot * 40.0);
	return LAMP_9000 * (0.45 + 0.45 * cut) * shimmer;
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
	float filtered = w < 0.05 ? (pulse_first(c + 0.5 * w, a0, a1) - pulse_first(c - 0.5 * w, a0, a1)) / w
			: (pulse_second(c + w, a0, a1) - 2.0 * pulse_second(c, a0, a1) + pulse_second(c - w, a0, a1)) / (w * w);
	return mix(filtered, mean, fade);
}
// The footprint as the gradient's length per axis: fwidth's |dx| + |dy| is up to 1.41 times it in an oblique view.
vec2 grid_footprint(vec2 cell) {
	vec2 dx = dFdx(cell), dy = dFdy(cell);
	return max(sqrt(dx * dx + dy * dy), vec2(1e-4));
}
