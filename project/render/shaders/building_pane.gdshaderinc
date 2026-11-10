// A pane of the near level: the opening it stands for, the room behind the glass by interior mapping (van Dongen, "Interior
// Mapping", CGI 2008), with the room's own size, floor and largest piece of furniture
// (native/src/render/buildings/pane_rooms.cpp), what hangs in the window, the frame in front of the glass, and the lamp of the
// room at night. Each hit is an exact ray-plane or slab test.
// Needs v_room0..2 and v_trim: room0 = back, ahead, depth, floor kind; room1 = sill, head, piece + 1, piece height;
// room2 = piece centre (signed), half width, near, far; trim = sRGB frame colour, frame thickness.
const float ROOM_SPAN_M = 12.75; // a byte's 1.0, metres (shell.h ROOM_SPAN_M)
const float FRAME_SPAN_M = 0.255; // a byte's 1.0 of the frame's thickness (shell.h FRAME_SPAN_M)
const float FRAME_SHARE = 0.045; // the border of the frame, of the pane's side (buildings.json parts.frame_bar_m)
const float FRAME_OUT_M = 0.03; // the frame's middle in front of the glass
const float TV_SHARE = 0.12; // the share of a home's lit rooms lit by a television alone
const float TILE_M = 0.6, BOARD_M = 0.14, PLANK_M = 2.4, PANEL_GRID_M = 2.4;
const float REVEAL_ALBEDO = 0.9; // the mean of the trim finishes' maps (render, stone, concrete)
const float REVEAL_SHADE = 0.68, SILL_SHADE = 0.92, HEAD_SHADE = 0.544; // facade_bay.cpp: the sides, the sill, the head

// The overlap of [a, b] with a pixel's footprint k round x, as a share of k.
float pane_span(float x, float k, float a, float b) { return clamp((min(x + 0.5 * k, b) - max(x - 0.5 * k, a)) / k, 0.0, 1.0); }

// The share of the pixel the frame covers and the face of it seen (pane axes, z towards the eye). Near, each bar is a box
// hit by the eye's ray; its bars under ~2 pixels, their area box-filtered over the pixel instead, so they neither shimmer
// nor vanish. `p` from the pane's middle, `ray` z into the wall.
// `hit`: how far along the ray back from the glass the bar was met, -1 when the frame was only filtered.
float pane_frame(vec2 p, vec3 ray, vec2 size, float thick, inout vec3 face, out float hit) {
	hit = -1.0;
	vec2 hs = 0.5 * size, t = FRAME_SHARE * size;
	vec4 bars[6];
	bars[0] = vec4(-hs.x, hs.x, hs.y - t.y, hs.y);
	bars[1] = vec4(-hs.x, hs.x, -hs.y, -hs.y + t.y);
	bars[2] = vec4(hs.x - t.x, hs.x, -hs.y, hs.y);
	bars[3] = vec4(-hs.x, -hs.x + t.x, -hs.y, hs.y);
	bars[4] = vec4(-0.4 * t.x, 0.4 * t.x, -hs.y, hs.y);
	bars[5] = vec4(-hs.x, hs.x, 0.18 * size.y - 0.4 * t.y, 0.18 * size.y + 0.4 * t.y);
	vec2 foot = max(fwidth(p), vec2(1e-5));
	float thin = min(0.8 * t.x / foot.x, 0.8 * t.y / foot.y);
	float cover_x = 0.0, cover_y = 0.0;
	for (int i = 0; i < 6; i++) {
		vec4 b = bars[i];
		if (b.y - b.x < size.x * 0.99) cover_x += pane_span(p.x, 1.5 * foot.x, b.x, b.y);
		else cover_y += pane_span(p.y, 1.5 * foot.y, b.z, b.w);
	}
	float filtered = 1.0 - (1.0 - clamp(cover_x, 0.0, 1.0)) * (1.0 - clamp(cover_y, 0.0, 1.0));
	if (thin < 1.0) return filtered;
	// Back from the glass towards the eye (s from 0): through each bar's box, the frame's depth in front of the glass.
	vec3 d = vec3(abs(ray.x) < 1e-5 ? 1e-5 : ray.x, abs(ray.y) < 1e-5 ? 1e-5 : ray.y, max(ray.z, 1e-5));
	float z_near = FRAME_OUT_M + 0.5 * thick, z_far = FRAME_OUT_M - 0.5 * thick;
	float sz0 = max(z_far, 0.0) / d.z, sz1 = z_near / d.z, best = -1.0;
	for (int i = 0; i < 6; i++) {
		vec4 b = bars[i];
		float sx0 = (p.x - b.y) / d.x, sx1 = (p.x - b.x) / d.x, sy0 = (p.y - b.w) / d.y, sy1 = (p.y - b.z) / d.y;
		vec2 sx = vec2(min(sx0, sx1), max(sx0, sx1)), sy = vec2(min(sy0, sy1), max(sy0, sy1));
		float lo = max(max(sx.x, sy.x), sz0), hi = min(min(sx.y, sy.y), sz1);
		if (lo <= hi && hi > best) {
			best = hi;
			face = hi == sz1 ? vec3(0.0, 0.0, 1.0) : hi == sx.y ? vec3(-sign(d.x), 0.0, 0.0) : vec3(0.0, -sign(d.y), 0.0);
		}
	}
	hit = best;
	return mix(filtered, best >= 0.0 ? 1.0 : 0.0, smoothstep(1.0, 2.0, thin));
}

// A pane on the wall's face, its glass `deep` behind it: where the eye's ray into the opening's box ends, each an exact
// ray-plane hit (Quilez, box functions; van Dongen 2008). Back: the share of the pixel on a side, the sill or the head, the
// edge filtered by the pixel, so a reveal under a pixel neither sparkles nor vanishes. `hit` the point met, `side` its outward
// normal, both on pane axes with z into the wall, `o` its place on the glass. The frame stands in front of the glass, so a side
// met behind the frame's front is the frame's. The depth stays the wall's: early depth is kept.
float recess_meet(vec3 d, vec2 size, float deep, float thick, inout vec3 o, out vec3 hit, out vec3 side, out float shade) {
	vec2 hs = 0.5 * size;
	float t_glass = deep / d.z;
	float tx = ((d.x > 0.0 ? hs.x : -hs.x) - o.x) / d.x, ty = ((d.y > 0.0 ? hs.y : -hs.y) - o.y) / d.y;
	float t_side = min(tx, ty);
	float t_front = thick > 0.0 ? max(deep - FRAME_OUT_M - 0.5 * thick, 0.0) / d.z : t_glass;
	float gap = t_front - t_side;
	float solid = clamp(gap / max(fwidth(gap), 1e-5) + 0.5, 0.0, 1.0);
	bool on_side = solid > 0.5;
	side = !on_side ? vec3(0.0, 0.0, -1.0) : tx < ty ? vec3(-sign(d.x), 0.0, 0.0) : vec3(0.0, d.y < 0.0 ? 1.0 : -1.0, 0.0);
	shade = tx < ty ? REVEAL_SHADE : d.y < 0.0 ? SILL_SHADE : HEAD_SHADE;
	hit = o + d * (on_side ? t_side : t_glass);
	o = vec3(hit.xy, 0.0);
	return solid;
}

// The share of the sky's hemisphere over `n` at `q` the opening lets through, cosine-weighted: Lambert's form factor of a
// polygon, exact, four edges (Baum, Rushmeier and Winget, "Improving radiosity solutions through the use of analytically
// determined form-factors", 1989).
vec3 form_edge(vec3 a, vec3 b) { return acos(clamp(dot(a, b), -1.0, 1.0)) * normalize(cross(a, b) + 1e-9); }
float opening_share(vec3 q, vec3 n, vec2 hs) {
	vec3 c0 = normalize(vec3(-hs.x, -hs.y, 0.0) - q), c1 = normalize(vec3(hs.x, -hs.y, 0.0) - q);
	vec3 c2 = normalize(vec3(hs.x, hs.y, 0.0) - q), c3 = normalize(vec3(-hs.x, hs.y, 0.0) - q);
	vec3 e = form_edge(c0, c1) + form_edge(c1, c2) + form_edge(c2, c3) + form_edge(c3, c0);
	return clamp(abs(dot(e, n)) / TAU, 0.0, 1.0);
}

vec3 room_floor(vec3 p, float kind) {
	if (kind > 1.5) { // porcelain tiles, thin joints
		vec2 tile = fract(p.xz / TILE_M);
		float joint = step(0.015, tile.x) * step(0.015, tile.y);
		return vec3(0.694, 0.651, 0.571) * mix(0.8, 1.0, joint) * (0.97 + 0.03 * h01(uint(floor(p.x / TILE_M) + 17.0 * floor(p.z / TILE_M) + 4096.0)));
	}
	if (kind > 0.5) { // stone tiles, wider joints
		vec2 tile = fract(p.xz / TILE_M);
		float joint = step(0.035, tile.x) * step(0.035, tile.y);
		return vec3(0.485, 0.445, 0.376) * mix(0.72, 1.0, joint) * (0.94 + 0.06 * h01(uint(floor(p.x / TILE_M) + 17.0 * floor(p.z / TILE_M) + 4096.0)));
	}
	float board = floor(p.x / BOARD_M); // floorboards running into the room, each its own shade
	float seam = step(0.06, fract(p.x / BOARD_M)) * step(0.02, fract((p.z + h01(uint(board + 4096.0)) * 3.0) / PLANK_M));
	return vec3(0.361, 0.178, 0.065) * mix(0.6, 1.0, seam) * (0.85 + 0.25 * h01(uint(board + 4096.0)));
}

// The room seen along `d` from `o` on the glass (pane axes), before what hangs in the window.
vec3 room_trace(vec3 o, vec3 d, float lot, vec3 lo, vec3 hi, float floor_kind) {
	vec3 t_far = max((lo - o) / d, (hi - o) / d);
	float t = min(min(t_far.x, t_far.y), t_far.z);
	vec3 p = o + d * t, c;
	float shade;
	if (t == t_far.y && d.y < 0.0) {
		c = room_floor(p, floor_kind);
		shade = 0.85;
	} else if (t == t_far.y) {
		c = vec3(0.905, 0.879, 0.823);
		shade = 0.9;
		if (floor_kind > 0.5 && floor_kind < 1.5) { // an office or a shop: its ceiling's light panels
			vec2 grid = fract(p.xz / PANEL_GRID_M);
			float panel = step(0.35, grid.x) * step(grid.x, 0.6) * step(0.25, grid.y) * step(grid.y, 0.75);
			c = mix(c, vec3(2.6, 2.55, 2.4), panel);
		}
	} else {
		c = vec3(0.838, 0.791, 0.701) * mix(0.62, 1.0, step(0.1, p.y - lo.y)); // a skirting along every wall
		shade = t == t_far.x ? 0.72 : 0.84;
		if (t == t_far.z && !(floor_kind > 0.5 && floor_kind < 1.5)) { // a home's back wall: a picture or a television
			float up = p.y - lo.y, h4 = fract(lot * 29.17), h5 = fract(lot * 41.3);
			float cx = (h4 - 0.5) * (hi.x - lo.x) * 0.4 + (hi.x + lo.x) * 0.5;
			if (h5 < 0.45) {
				float wide = 0.35 + 0.4 * h4;
				float inside = step(abs(p.x - cx), wide) * step(abs(up - 1.6), 0.3 + 0.15 * h5);
				float rim = inside * (1.0 - step(abs(p.x - cx), wide - 0.04) * step(abs(up - 1.6), 0.26 + 0.15 * h5));
				c = mix(c, mix(mix(vec3(0.55, 0.32, 0.2), vec3(0.25, 0.4, 0.55), h4), vec3(0.12, 0.1, 0.08), rim), inside);
			} else if (h5 < 0.7) {
				c = mix(c, vec3(0.02, 0.02, 0.025), step(abs(p.x - cx), 0.55) * step(abs(up - 1.25), 0.32));
			}
		}
	}
	shade *= mix(1.0, 0.42, clamp(p.z / 7.0, 0.0, 1.0)); // the daylight falls off into the room
	vec3 edge = min(p - lo, hi - p);
	float near_edge = t == t_far.x ? min(edge.y, edge.z) : t == t_far.y ? min(edge.x, edge.z) : min(edge.x, edge.y);
	shade *= mix(0.55, 1.0, smoothstep(0.0, 0.6, near_edge));
	float kind = v_room1.b * 255.0 - 0.5;
	if (kind > -0.25) { // the piece of furniture: a box on the floor
		vec3 fc = vec3((v_room2.r * 2.0 - 1.0) * ROOM_SPAN_M, lo.y, v_room2.b * ROOM_SPAN_M);
		vec3 flo = vec3(fc.x - v_room2.g * ROOM_SPAN_M, lo.y, fc.z);
		vec3 fhi = vec3(fc.x + v_room2.g * ROOM_SPAN_M, lo.y + v_room1.a * ROOM_SPAN_M, v_room2.a * ROOM_SPAN_M);
		vec3 fa = (flo - o) / d, fb = (fhi - o) / d, fn = min(fa, fb), ff = max(fa, fb);
		float tn = max(max(fn.x, fn.y), fn.z), tf = min(min(ff.x, ff.y), ff.z);
		if (tn <= tf && tn > 0.0 && tn < t) {
			vec3 q = o + d * tn;
			int k = int(kind + 0.5) * 3;
			c = vec3(furniture_tone[k], furniture_tone[k + 1], furniture_tone[k + 2]);
			shade = (tn == fn.y ? 0.95 : tn == fn.x ? 0.62 : 0.78) * mix(1.0, 0.45, clamp(q.z / 7.0, 0.0, 1.0));
			shade *= mix(0.7, 1.0, smoothstep(0.0, 0.25, q.y - flo.y)); // its foot in its own shadow
		}
	}
	return c * shade * 0.9;
}

// What hangs 12 cm behind the glass, each room its own: curtains drawn to the sides, a roller blind, a Venetian blind, a
// sheer. Mixes into `room`.
vec3 room_hung(vec3 room, vec3 o, vec3 d, vec2 size, float lot) {
	float h1 = fract(lot * 13.71), h2 = fract(lot * 7.37), h3 = fract(lot * 3.13);
	vec3 cq = o + d * ((0.12 - o.z) / d.z);
	vec2 pu = cq.xy / max(size, vec2(0.1)) + 0.5;
	vec3 cloth = pane_cloth(h2);
	float cover = 0.0, daylight = 0.85 + 0.25 * clamp(pu.y, 0.0, 1.0);
	vec3 hung = cloth;
	if (h1 < 0.36) {
		float side = 0.14 + 0.2 * h3, sway = 0.03 * sin(pu.y * 7.0 + h2 * 6.0);
		cover = clamp(step(pu.x, side + sway) + step(1.0 - side - sway, pu.x), 0.0, 1.0);
		float fold = sin(pu.x * size.x * 70.0 + h3 * 3.0);
		hung = cloth * (0.72 + 0.28 * (0.5 + 0.5 * fold) * (0.6 + 0.4 * fold)) * mix(0.82, 1.0, step(0.04, pu.y)) * daylight;
	} else if (h1 < 0.56) {
		float down = 0.15 + 0.65 * h3;
		cover = step(1.0 - down, pu.y);
		hung = mix(cloth * daylight * 0.92, vec3(0.2), step(abs(pu.y - (1.0 - down)), 0.012));
	} else if (h1 < 0.68) {
		float slat = 0.5 + 0.5 * sin(pu.y * size.y * 62.0);
		cover = smoothstep(0.3, 0.6, slat) * 0.9;
		hung = vec3(0.78, 0.77, 0.73) * (0.75 + 0.25 * slat) * daylight;
	} else if (h1 < 0.8) {
		cover = 0.4 + 0.2 * (0.5 + 0.5 * sin(pu.x * size.x * 55.0 + h3 * 5.0));
		hung = vec3(0.9, 0.89, 0.86) * daylight;
	}
	return mix(room, hung * 0.8, cover);
}

// The lamp of a room at night, multiplied by what the room shows. Who is awake is the room's; whether this stretch of it is lit and how much is `lot`, a bay's, as the
// 4.0's vRoomLot is of what stands within 2.7 m: a wide floor is lit in patches, never all or nothing.
vec3 pane_lamp(uint slot, float lot, float office, vec3 seen) {
	float awake = step(1.0 - rooms_awake, h01(slot * 2654435761u + 977u));
	float level = 0.3 + 0.85 * fract(lot * 7.13) * fract(lot * 2.17 + 0.4);
	float lit = step(fract(lot * 5.31), 0.64) * awake * level * building_night_windows;
	vec3 tint = room_lamp(fract(lot * 17.31), office);
	tint = mix(tint, tv_light(TIME, lot), (1.0 - office) * step(1.0 - TV_SHARE, fract(lot * 3.71)));
	return tint * lit * (vec3(0.35) + 1.3 * clamp(seen / 0.55, vec3(0.0), vec3(1.6)));
}

// A pane: uv 0..1 over the glass, `size` metres, `slot` its room's seed, `bay` its own. Out: what the surface is.
// `deep`: the opening behind the wall's face, 0 for a pane at its glass. Out too: `at` the point lit on pane axes, `open` the
// share of the sky it sees.
void pane(vec2 uv, vec2 size, uint slot, uint bay, bool dark, float deep, vec3 n, vec3 V, out vec3 albedo, out vec3 emit, out vec3 nn, out float rough, out float metal, out vec3 at, out float open) {
	vec3 t = normalize(vec3(n.z, 0.0, -n.x));
	float lot = h01(bay * 7919u + slot * 31u + 1u);
	vec3 ray = vec3(dot(V, t), V.y, max(dot(V, -n), 1e-4));
	vec3 d = vec3(abs(ray.x) < 1e-4 ? 1e-4 : ray.x, abs(ray.y) < 1e-4 ? 1e-4 : ray.y, ray.z);
	vec3 o = vec3((uv - 0.5) * size, 0.0);
	float tiny = smoothstep(0.05, 0.3, max(fwidth(uv.x), fwidth(uv.y)));
	float thick = v_trim.a * FRAME_SPAN_M;
	float solid = 0.0, side_shade = 1.0;
	vec3 side = vec3(0.0, 0.0, -1.0);
	at = vec3(0.0);
	if (deep > 0.0) solid = recess_meet(d, size, deep, thick, o, at, side, side_shade);
	vec3 face = vec3(0.0, 0.0, 1.0);
	float frame_hit = -1.0;
	float frame = thick > 0.0 ? pane_frame(o.xy, ray, size, thick, face, frame_hit) * (1.0 - solid) : 0.0;
	open = 1.0;
	if (deep > 0.0) {
		if (frame_hit >= 0.0 && frame > 0.5) {
			at = vec3(o.xy - ray.xy * frame_hit, deep - ray.z * frame_hit);
			side = vec3(face.xy, -face.z);
		}
		at += side * 1e-3; // off the surface a hair: a point on a side sees the opening's edge in that side's plane on the right side
		open = opening_share(at, side, 0.5 * size);
	}
	float kind = v_room0.a * 255.0;
	vec3 seen = pane_tone(lot, room_tone(V, n, kind > 1.5 ? FLOOR_TILE : kind > 0.5 ? FLOOR_STONE : FLOOR_BOARD));
	if (tiny < 1.0 && frame < 1.0 && solid < 1.0) {
		vec3 lo = vec3(-v_room0.r, -v_room1.r, 0.0) * ROOM_SPAN_M, hi = vec3(v_room0.g, v_room1.g, v_room0.b) * ROOM_SPAN_M;
		vec3 room = room_hung(room_trace(o, d, lot, lo, hi, kind), o, d, size, lot);
		seen = mix(room, seen, tiny);
	}
	// Less of the room as the glass turns to a mirror (Schlick, F0 4 %): at every size, or a pane seen edge-on burns as a bright sliver.
	float seen_cos = clamp(ray.z / max(length(ray), 1e-4), 0.0, 1.0);
	seen *= 1.0 - (0.04 + 0.96 * pow(1.0 - seen_cos, 5.0));
	float office = kind > 0.5 && kind < 1.5 ? 1.0 : 0.0, glass_k = dark ? 0.64 : 1.0;
	float cosv = clamp(dot(-V, n), 0.0, 1.0);
	float F = max(0.04 + 0.96 * pow(1.0 - cosv, 5.0), glass_reflect);
	// The glass scatters next to nothing: what a pane shows is the room through it and the sky's reflection, as light leaving it.
	vec3 refl = glass_reflection(V, n, F, dark ? GLASS_ENV_DARK : GLASS_ENV);
	vec3 trim = to_linear(v_trim.rgb);
	albedo = mix(mix(vec3(0.0075, 0.0096, 0.0115) * glass_k, trim, frame), trim * REVEAL_ALBEDO * side_shade, solid);
	emit = (seen * room_daylight * glass_k + pane_lamp(slot, lot, office, seen) * night * window_glow * glass_k + refl * open) * (1.0 - max(frame, solid));
	nn = normalize(mix(n, normalize(t * face.x + vec3(0.0, face.y, 0.0) + n * face.z), step(0.5, frame)));
	nn = normalize(mix(nn, normalize(t * side.x + vec3(0.0, side.y, 0.0) - n * side.z), step(0.5, solid)));
	rough = mix(mix(mix(0.12, 0.55, tiny), 0.55, frame), 0.9, solid);
	metal = mix(mix(0.0, 0.05, frame), 0.0, solid);
}

// The glass of a building cut open: nothing behind it to trace (the inside is real geometry), only what the glass reflects.
void pane_clear(vec3 n, vec3 V, out vec3 albedo, out vec3 emit, out vec3 nn, out float rough, out float metal) {
	float cosv = clamp(dot(-V, n), 0.0, 1.0);
	float F = max(0.04 + 0.96 * pow(1.0 - cosv, 5.0), glass_reflect);
	albedo = vec3(0.0075, 0.0096, 0.0115);
	emit = glass_reflection(V, n, F, GLASS_ENV);
	nn = n;
	rough = 0.12;
	metal = 0.0;
}
