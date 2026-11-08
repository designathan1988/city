// A pane of the near level: the opening it stands for, the room behind the glass by interior mapping (van Dongen, "Interior
// Mapping", CGI 2008), with the room's own size, floor and largest piece of furniture
// (native/src/render/buildings/pane_rooms.cpp), what hangs in the window, the frame in front of the glass, and the lamp of the
// room at night. Each hit is an exact ray-plane or slab test.
// Needs v_room0..2 and v_trim: room0 = back, ahead, depth, floor kind; room1 = sill, head, piece + 1, piece height;
// room2 = piece centre (signed), half width, near, far; trim = sRGB frame colour, frame thickness.
#include "res://render/shaders/contracts/buildings.gdshaderinc"
#include "res://render/shaders/fresnel.gdshaderinc"
#include "res://render/shaders/bytes.gdshaderinc"
const float FRAME_SHARE = 0.045; // the border of the frame, of the pane's side (buildings.json parts.frame_bar_m)
const float REVEAL_ALBEDO = 0.9; // the mean of the trim finishes' maps (render, stone, concrete)
const int FRAME_BARS = 6, TRANSOM = 5; // the frame's bars: its four sides, the mullion (4) and the transom
const float FULL_SPAN = 0.99; // a bar this much of the pane's width across lies along it (the share only keeps rounding out)
// Guards, far under a pixel: a footprint or a ray's part never under LEAST_M (LEAST_RAY into a pane), normalize never of two
// corners seen in one line, a lit point off its surface by a hair, what hangs laid on a pane of at least LEAST_PANE_M.
const float LEAST_M = 1e-5, LEAST_RAY = 1e-4, LEAST_CROSS = 1e-9, OFF_SURFACE_M = 1e-3, LEAST_PANE_M = 0.1;
// A floor cell's seed: its column plus its row times CELL_ROW (more than a room's tiles a row), offset to stay positive.
const float CELL_ROW = 17.0, CELL_OFFSET = 4096.0;
// A room's independent picks from its lot: fract(lot * k), each k far from a simple ratio of the others.
const float LOT_PICTURE = 29.17, LOT_ART = 41.3, LOT_LIT = 5.31, LOT_TV = 3.71; // (LOT_HUNG, LOT_CLOTH, LOT_SIDE: building_room_tone)
// A room's and a bay's seeds: Knuth's multiplicative hash (hash.gdshaderinc), a salt, and two primes.
const uint AWAKE_SALT = 977u, BAY_PRIME = 7919u, SLOT_PRIME = 31u;

// The room and what hangs in its window: data/render/materials.json building_glass.params, by eye (the 4.0's rooms). Colours are
// linear albedo, lengths metres; a pick of h (a lot) is from + over * h.
uniform float frame_out_m; // the frame's middle in front of the glass
uniform vec2 frame_cross; // the middle bars' half width, of the border; the transom's height over the middle, of the pane's
uniform float frame_thin; // a bar's width the pixel filter counts, of the border
uniform float tv_share; // the share of a home's lit rooms lit by a television alone
uniform vec4 floor_sizes; // a tile's side, a floorboard's width, a plank's length, the ceiling panels' grid
uniform vec4 tile_look; // porcelain tiles: the joint, of the tile; the joint's shade; each tile's shade, from and over
uniform vec3 tile_albedo;
uniform vec4 stone_look; // stone tiles, the same
uniform vec3 stone_albedo;
uniform vec4 board_look; // floorboards: the seam, of the board; the seam's shade; each board's shade, from and over
uniform float board_end; // a plank's end joint, of its length
uniform vec3 board_albedo;
uniform vec4 room_shades; // the light on the floor, the ceiling, a side wall, the back wall
uniform vec3 ceiling_albedo;
uniform vec4 panel_box; // an office's ceiling panel in its grid cell: from and to across, from and to along
uniform vec3 panel_light; // its radiance
uniform vec3 wall_albedo;
uniform vec2 skirting; // its shade, its height
uniform vec2 art_kinds; // a home's back wall: a picture under the first share of lots, a television under the second
uniform float art_spread; // how far off the middle it hangs, of the room's width
uniform vec2 art_height; // the picture's middle, the television's
uniform vec4 picture_size; // its half width, from and over; its half height, from and over
uniform vec2 picture_rim; // its frame's width at the sides, the canvas's half height
uniform vec3 canvas_a; // a canvas's tone between these two
uniform vec3 canvas_b;
uniform vec3 picture_frame;
uniform vec2 tv_size; // a television's half width and half height
uniform vec3 tv_albedo;
uniform vec3 daylight_fall; // the daylight falls off into the room over this depth, to these shares on the walls and the furniture
uniform vec2 corner_shade; // a wall's shade at a corner, the distance it lifts over
uniform vec3 piece_shades; // the furniture: its top, a side, its front
uniform vec2 piece_foot; // its foot's shade, the height it lifts over
uniform float room_dim; // the room's light, all of it
uniform float hung_back_m; // what hangs, behind the glass
uniform vec2 hung_daylight; // its light, from and over the height up the pane
uniform vec3 curtain_sway; // its sway: how far, its waves up the pane, the lot's phase
uniform float curtain_folds; // its folds a metre (by 2 pi)
uniform vec4 curtain_shade; // a fold's shade: from and over, its two-sided lift from and over
uniform vec2 curtain_hem; // the hem's shade, its height
uniform vec3 roller_look; // its shade, its bar's grey, the bar's half height
uniform vec4 venetian_look; // a Venetian blind's slats a metre (by 2 pi), the cover's edges, the cover
uniform vec3 venetian_albedo;
uniform vec2 venetian_shade; // a slat's shade, from and over
uniform vec4 sheer_look; // a sheer's cover, from and over; its waves a metre (by 2 pi), the lot's phase
uniform vec3 sheer_albedo;
uniform float hung_dim; // what hangs, its light
uniform vec2 tiny_edges; // the room is traced under the first footprint of the pane's uv a pixel and given up over the second
uniform vec3 pane_rough; // the roughness of the glass, of a pane too small to trace and of the frame, of the reveal
uniform float frame_metal;

// The overlap of [a, b] with a pixel's footprint k round x, as a share of k.
float pane_span(float x, float k, float a, float b) { return clamp((min(x + 0.5 * k, b) - max(x - 0.5 * k, a)) / k, 0.0, 1.0); }

// The share of the pixel the frame covers and the face of it seen (pane axes, z towards the eye). Near, each bar is a box
// hit by the eye's ray; its bars under ~2 pixels, their area box-filtered over the pixel instead, so they neither shimmer
// nor vanish. `p` from the pane's middle, `ray` z into the wall.
// `hit`: how far along the ray back from the glass the bar was met, -1 when the frame was only filtered.
float pane_frame(vec2 p, vec3 ray, vec2 size, float thick, inout vec3 face, out float hit) {
	hit = -1.0;
	vec2 hs = 0.5 * size, t = FRAME_SHARE * size;
	vec4 bars[FRAME_BARS];
	bars[0] = vec4(-hs.x, hs.x, hs.y - t.y, hs.y);
	bars[1] = vec4(-hs.x, hs.x, -hs.y, -hs.y + t.y);
	bars[2] = vec4(hs.x - t.x, hs.x, -hs.y, hs.y);
	bars[3] = vec4(-hs.x, -hs.x + t.x, -hs.y, hs.y);
	bars[4] = vec4(-frame_cross.x * t.x, frame_cross.x * t.x, -hs.y, hs.y);
	bars[TRANSOM] = vec4(-hs.x, hs.x, frame_cross.y * size.y - frame_cross.x * t.y, frame_cross.y * size.y + frame_cross.x * t.y);
	vec2 foot = max(fwidth(p), vec2(LEAST_M));
	float thin = min(frame_thin * t.x / foot.x, frame_thin * t.y / foot.y);
	float cover_x = 0.0, cover_y = 0.0;
	for (int i = 0; i < FRAME_BARS; i++) {
		vec4 b = bars[i];
		if (b.y - b.x < size.x * FULL_SPAN) cover_x += pane_span(p.x, 1.5 * foot.x, b.x, b.y);
		else cover_y += pane_span(p.y, 1.5 * foot.y, b.z, b.w);
	}
	float filtered = 1.0 - (1.0 - clamp(cover_x, 0.0, 1.0)) * (1.0 - clamp(cover_y, 0.0, 1.0));
	if (thin < 1.0) return filtered;
	// Back from the glass towards the eye (s from 0): through each bar's box, the frame's depth in front of the glass.
	vec3 d = vec3(abs(ray.x) < LEAST_M ? LEAST_M : ray.x, abs(ray.y) < LEAST_M ? LEAST_M : ray.y, max(ray.z, LEAST_M));
	float z_near = frame_out_m + 0.5 * thick, z_far = frame_out_m - 0.5 * thick;
	float sz0 = max(z_far, 0.0) / d.z, sz1 = z_near / d.z, best = -1.0;
	for (int i = 0; i < FRAME_BARS; i++) {
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
	float t_front = thick > 0.0 ? max(deep - frame_out_m - 0.5 * thick, 0.0) / d.z : t_glass;
	float gap = t_front - t_side;
	float solid = clamp(gap / max(fwidth(gap), LEAST_M) + 0.5, 0.0, 1.0);
	bool on_side = solid > 0.5;
	side = !on_side ? vec3(0.0, 0.0, -1.0) : tx < ty ? vec3(-sign(d.x), 0.0, 0.0) : vec3(0.0, d.y < 0.0 ? 1.0 : -1.0, 0.0);
	shade = tx < ty ? REVEAL_SHADE : d.y < 0.0 ? SILL_SHADE : REVEAL_SHADE * HEAD_SHADE; // the head, of its sides' shade
	hit = o + d * (on_side ? t_side : t_glass);
	o = vec3(hit.xy, 0.0);
	return solid;
}

// The share of the sky's hemisphere over `n` at `q` the opening lets through, cosine-weighted: Lambert's form factor of a
// polygon, exact, four edges (Baum, Rushmeier and Winget, "Improving radiosity solutions through the use of analytically
// determined form-factors", 1989).
vec3 form_edge(vec3 a, vec3 b) { return acos(clamp(dot(a, b), -1.0, 1.0)) * normalize(cross(a, b) + LEAST_CROSS); }
float opening_share(vec3 q, vec3 n, vec2 hs) {
	vec3 c0 = normalize(vec3(-hs.x, -hs.y, 0.0) - q), c1 = normalize(vec3(hs.x, -hs.y, 0.0) - q);
	vec3 c2 = normalize(vec3(hs.x, hs.y, 0.0) - q), c3 = normalize(vec3(-hs.x, hs.y, 0.0) - q);
	vec3 e = form_edge(c0, c1) + form_edge(c1, c2) + form_edge(c2, c3) + form_edge(c3, c0);
	return clamp(abs(dot(e, n)) / TAU, 0.0, 1.0);
}

vec3 room_floor(vec3 p, float kind) {
	if (kind > 1.5) { // porcelain tiles, thin joints
		vec2 tile = fract(p.xz / floor_sizes.x);
		float joint = step(tile_look.x, tile.x) * step(tile_look.x, tile.y);
		return tile_albedo * mix(tile_look.y, 1.0, joint) * (tile_look.z + tile_look.w * h01(uint(floor(p.x / floor_sizes.x) + CELL_ROW * floor(p.z / floor_sizes.x) + CELL_OFFSET)));
	}
	if (kind > 0.5) { // stone tiles, wider joints
		vec2 tile = fract(p.xz / floor_sizes.x);
		float joint = step(stone_look.x, tile.x) * step(stone_look.x, tile.y);
		return stone_albedo * mix(stone_look.y, 1.0, joint) * (stone_look.z + stone_look.w * h01(uint(floor(p.x / floor_sizes.x) + CELL_ROW * floor(p.z / floor_sizes.x) + CELL_OFFSET)));
	}
	float board = floor(p.x / floor_sizes.y); // floorboards running into the room, each its own shade
	float seam = step(board_look.x, fract(p.x / floor_sizes.y)) * step(board_end, fract((p.z + h01(uint(board + CELL_OFFSET)) * 3.0) / floor_sizes.z));
	return board_albedo * mix(board_look.y, 1.0, seam) * (board_look.z + board_look.w * h01(uint(board + CELL_OFFSET)));
}

// The room seen along `d` from `o` on the glass (pane axes), before what hangs in the window.
vec3 room_trace(vec3 o, vec3 d, float lot, vec3 lo, vec3 hi, float floor_kind) {
	vec3 t_far = max((lo - o) / d, (hi - o) / d);
	float t = min(min(t_far.x, t_far.y), t_far.z);
	vec3 p = o + d * t, c;
	float shade;
	if (t == t_far.y && d.y < 0.0) {
		c = room_floor(p, floor_kind);
		shade = room_shades.x;
	} else if (t == t_far.y) {
		c = ceiling_albedo;
		shade = room_shades.y;
		if (floor_kind > 0.5 && floor_kind < 1.5) { // an office or a shop: its ceiling's light panels
			vec2 grid = fract(p.xz / floor_sizes.w);
			float panel = step(panel_box.x, grid.x) * step(grid.x, panel_box.y) * step(panel_box.z, grid.y) * step(grid.y, panel_box.w);
			c = mix(c, panel_light, panel);
		}
	} else {
		c = wall_albedo * mix(skirting.x, 1.0, step(skirting.y, p.y - lo.y)); // a skirting along every wall
		shade = t == t_far.x ? room_shades.z : room_shades.w;
		if (t == t_far.z && !(floor_kind > 0.5 && floor_kind < 1.5)) { // a home's back wall: a picture or a television
			float up = p.y - lo.y, h4 = fract(lot * LOT_PICTURE), h5 = fract(lot * LOT_ART);
			float cx = (h4 - 0.5) * (hi.x - lo.x) * art_spread + (hi.x + lo.x) * 0.5;
			if (h5 < art_kinds.x) {
				float wide = picture_size.x + picture_size.y * h4;
				float inside = step(abs(p.x - cx), wide) * step(abs(up - art_height.x), picture_size.z + picture_size.w * h5);
				float rim = inside * (1.0 - step(abs(p.x - cx), wide - picture_rim.x) * step(abs(up - art_height.x), picture_rim.y + picture_size.w * h5));
				c = mix(c, mix(mix(canvas_a, canvas_b, h4), picture_frame, rim), inside);
			} else if (h5 < art_kinds.y) {
				c = mix(c, tv_albedo, step(abs(p.x - cx), tv_size.x) * step(abs(up - art_height.y), tv_size.y));
			}
		}
	}
	shade *= mix(1.0, daylight_fall.y, clamp(p.z / daylight_fall.x, 0.0, 1.0)); // the daylight falls off into the room
	vec3 edge = min(p - lo, hi - p);
	float near_edge = t == t_far.x ? min(edge.y, edge.z) : t == t_far.y ? min(edge.x, edge.z) : min(edge.x, edge.y);
	shade *= mix(corner_shade.x, 1.0, smoothstep(0.0, corner_shade.y, near_edge));
	float kind = v_room1.b * BYTE_TOP - 0.5;
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
			shade = (tn == fn.y ? piece_shades.x : tn == fn.x ? piece_shades.y : piece_shades.z) * mix(1.0, daylight_fall.z, clamp(q.z / daylight_fall.x, 0.0, 1.0));
			shade *= mix(piece_foot.x, 1.0, smoothstep(0.0, piece_foot.y, q.y - flo.y)); // its foot in its own shadow
		}
	}
	return c * shade * room_dim;
}

// What hangs `hung_back_m` behind the glass, each room its own: curtains drawn to the sides, a roller blind, a Venetian blind,
// a sheer. Mixes into `room`.
vec3 room_hung(vec3 room, vec3 o, vec3 d, vec2 size, float lot) {
	float h1 = fract(lot * LOT_HUNG), h2 = fract(lot * LOT_CLOTH), h3 = fract(lot * LOT_SIDE);
	vec3 cq = o + d * ((hung_back_m - o.z) / d.z);
	vec2 pu = cq.xy / max(size, vec2(LEAST_PANE_M)) + 0.5;
	vec3 cloth = pane_cloth(h2);
	float cover = 0.0, daylight = hung_daylight.x + hung_daylight.y * clamp(pu.y, 0.0, 1.0);
	vec3 hung = cloth;
	if (h1 < hung_kinds.x) {
		float side = curtain_side.x + curtain_side.y * h3, sway = curtain_sway.x * sin(pu.y * curtain_sway.y + h2 * curtain_sway.z);
		cover = clamp(step(pu.x, side + sway) + step(1.0 - side - sway, pu.x), 0.0, 1.0);
		float fold = sin(pu.x * size.x * curtain_folds + h3 * 3.0);
		hung = cloth * (curtain_shade.x + curtain_shade.y * (0.5 + 0.5 * fold) * (curtain_shade.z + curtain_shade.w * fold)) * mix(curtain_hem.x, 1.0, step(curtain_hem.y, pu.y)) * daylight;
	} else if (h1 < hung_kinds.y) {
		float down = roller_down.x + roller_down.y * h3;
		cover = step(1.0 - down, pu.y);
		hung = mix(cloth * daylight * roller_look.x, vec3(roller_look.y), step(abs(pu.y - (1.0 - down)), roller_look.z));
	} else if (h1 < hung_kinds.z) {
		float slat = 0.5 + 0.5 * sin(pu.y * size.y * venetian_look.x);
		cover = smoothstep(venetian_look.y, venetian_look.z, slat) * venetian_look.w;
		hung = venetian_albedo * (venetian_shade.x + venetian_shade.y * slat) * daylight;
	} else if (h1 < hung_kinds.w) {
		cover = sheer_look.x + sheer_look.y * (0.5 + 0.5 * sin(pu.x * size.x * sheer_look.z + h3 * sheer_look.w));
		hung = sheer_albedo * daylight;
	}
	return mix(room, hung * hung_dim, cover);
}

// The lamp of a room at night: the room's surfaces as it lights them, its working plane's illuminance (room_light_level) times what
// they show. Who is awake is the room's (an office's lights follow its schedule); whether this stretch of it is lit and how much is `lot`, a bay's, as the
// 4.0's vRoomLot is of what stands within 2.7 m: a wide floor is lit in patches, never all or nothing.
vec3 pane_lamp(uint slot, float lot, float office, vec3 seen) {
	float awake = step(1.0 - (office > 0.5 ? offices_lit : rooms_awake), h01(slot * KNUTH_HASH + AWAKE_SALT));
	float in_use = office > 0.5 ? 1.0 : step(fract(lot * LOT_LIT), rooms_in_use);
	float lit = in_use * awake * room_level(lot) * building_night_windows;
	vec3 tint = room_lamp(lot, office);
	tint = mix(tint, tv_light(TIME, lot), (1.0 - office) * step(1.0 - tv_share, fract(lot * LOT_TV)));
	return tint * lit * seen * room_light_level(office);
}

// A pane: uv 0..1 over the glass, `size` metres, `slot` its room's seed, `bay` its own. Out: what the surface is.
// `deep`: the opening behind the wall's face, 0 for a pane at its glass. Out too: `at` the point lit on pane axes, `open` the
// share of the sky it sees.
void pane(vec2 uv, vec2 size, uint slot, uint bay, bool dark, float deep, vec3 n, vec3 V, out vec3 albedo, out vec3 emit, out vec3 nn, out float rough, out float metal, out vec3 at, out float open) {
	vec3 t = normalize(vec3(n.z, 0.0, -n.x));
	float lot = h01(bay * BAY_PRIME + slot * SLOT_PRIME + 1u);
	vec3 ray = vec3(dot(V, t), V.y, max(dot(V, -n), LEAST_RAY));
	vec3 d = vec3(abs(ray.x) < LEAST_RAY ? LEAST_RAY : ray.x, abs(ray.y) < LEAST_RAY ? LEAST_RAY : ray.y, ray.z);
	vec3 o = vec3((uv - 0.5) * size, 0.0);
	float tiny = smoothstep(tiny_edges.x, tiny_edges.y, max(fwidth(uv.x), fwidth(uv.y)));
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
		at += side * OFF_SURFACE_M; // a point on a side sees the opening's edge in that side's plane on the right side
		open = opening_share(at, side, 0.5 * size);
	}
	float kind = v_room0.a * BYTE_TOP;
	vec3 seen = pane_tone(lot, room_tone(V, n, kind > 1.5 ? FLOOR_TILE : kind > 0.5 ? FLOOR_STONE : FLOOR_BOARD));
	if (tiny < 1.0 && frame < 1.0 && solid < 1.0) {
		vec3 lo = vec3(-v_room0.r, -v_room1.r, 0.0) * ROOM_SPAN_M, hi = vec3(v_room0.g, v_room1.g, v_room0.b) * ROOM_SPAN_M;
		vec3 room = room_hung(room_trace(o, d, lot, lo, hi, kind), o, d, size, lot);
		seen = mix(room, seen, tiny);
	}
	// Less of the room as the glass turns to a mirror (Schlick at the glazing's F0): at every size, or a pane seen edge-on burns as a
	// bright sliver.
	float seen_cos = clamp(ray.z / max(length(ray), LEAST_RAY), 0.0, 1.0);
	float office = kind > 0.5 && kind < 1.5 ? 1.0 : 0.0, glass_k = dark ? dark_glass : 1.0;
	float f0 = glazing_f0(office);
	seen *= glazing_through(f0, seen_cos);
	// The glass scatters next to nothing: what a pane shows is the room through it, as light leaving it, and what it mirrors (the
	// engine's reflections of the sky and the street, Fresnel's share at this angle).
	vec3 trim = srgb_to_linear(v_trim.rgb);
	albedo = mix(mix(vec3(f0), trim, frame), trim * REVEAL_ALBEDO * side_shade, solid);
	emit = (seen * room_daylight() + pane_lamp(slot, lot, office, seen) * night) * glass_k * (1.0 - max(frame, solid));
	nn = normalize(mix(n, normalize(t * face.x + vec3(0.0, face.y, 0.0) + n * face.z), step(0.5, frame)));
	nn = normalize(mix(nn, normalize(t * side.x + vec3(0.0, side.y, 0.0) - n * side.z), step(0.5, solid)));
	rough = mix(mix(mix(pane_rough.x, pane_rough.y, tiny), pane_rough.y, frame), pane_rough.z, solid);
	metal = mix(mix(1.0, frame_metal, frame), 0.0, solid);
}

// The glass of a building cut open: nothing behind it to trace (the inside is real geometry), only what the glass mirrors.
void pane_clear(vec3 n, vec3 V, out vec3 albedo, out vec3 emit, out vec3 nn, out float rough, out float metal) {
	albedo = vec3(glazing_reflect);
	emit = vec3(0.0);
	nn = n;
	rough = pane_rough.x;
	metal = 1.0;
}
