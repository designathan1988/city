// What a lit window is, for every level of the buildings: the lamp of a room, who is lit, and the filtered window grid.
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

vec3 room_lamp(float lot, float office) {
	if (office > 0.5) return lot < 0.5 ? LAMP_4000 : lot < 0.85 ? LAMP_5000 : LAMP_3500;
	return lot < 0.36 ? LAMP_2700 : lot < 0.6 ? LAMP_3000 : lot < 0.7 ? LAMP_3500 : LAMP_6500;
}
vec3 room_lamp_mean(float office) { return office > 0.5 ? LAMP_OFFICE_MEAN : LAMP_HOME_MEAN; }

// A television alone in the room: the cuts of a picture, about one every 1.7 s, and the small flicker over them.
vec3 tv_light(float clock, float lot) {
	float cut = h01(uint(clock * 0.6 + lot * 37.0) * 2654435761u + uint(lot * 8191.0));
	float shimmer = 0.92 + 0.08 * sin(clock * 11.0 + lot * 40.0);
	return LAMP_9000 * (0.45 + 0.45 * cut) * shimmer;
}

// Who is lit, the one rule of every level. A room is awake or asleep as one (`slot`, its seed: pane_rooms.cpp pane_seed); of an awake
// room's panes LIT_SHARE are lit, each at its own level and lamp, a television alone in TV_SHARE of the homes' (`lot`, the pane's own,
// of its room and its bay byte): a wide floor is lit in patches, never all or nothing. A level that draws a pane of its own takes
// pane_light; one that draws many under a pixel, or cells with no room known, the mean of it.
const float LIT_SHARE = 0.64;
const float LIT_LEVEL_MEAN = 0.5125; // the mean of pane_level: 0.3 + 0.85 x the mean of two lots' product, a quarter
const float TV_SHARE = 0.12;
float pane_lot(uint slot, uint bay) { return h01(bay * 7919u + slot * 31u + 1u); }
float pane_level(float lot) { return 0.3 + 0.85 * fract(lot * 7.13) * fract(lot * 2.17 + 0.4); }
vec3 pane_light(uint slot, float lot, float office) {
	float awake = step(1.0 - rooms_awake, h01(slot * 2654435761u + 977u));
	float lit = step(fract(lot * 5.31), LIT_SHARE) * awake * pane_level(lot);
	vec3 tint = room_lamp(fract(lot * 17.31), office);
	tint = mix(tint, tv_light(TIME, lot), (1.0 - office) * step(1.0 - TV_SHARE, fract(lot * 3.71)));
	return tint * lit;
}
// A cell with no room known (a far wall's): lit by its own lot, at the rule's share and mean level.
vec3 cell_light(float lot, float office) { return room_lamp(fract(lot * 17.31), office) * step(1.0 - rooms_awake * LIT_SHARE, lot) * LIT_LEVEL_MEAN; }
vec3 pane_light_mean(float office) { return room_lamp_mean(office) * rooms_awake * LIT_SHARE * LIT_LEVEL_MEAN; }
// A lamp's light as it leaves its window: over what the room shows through the glass (`seen`).
vec3 lamp_on_room(vec3 light, vec3 seen) { return light * (vec3(0.35) + 1.3 * clamp(seen / 0.55, vec3(0.0), vec3(1.6))); }

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
