// What a room shows through its glass when it is too small to trace (building_pane.gdshaderinc room_trace, room_hung): every level draws
// the same room, or a facade changes colour as the level changes. The tones are room_trace's surfaces under its own shades: the
// daylight's fall at 2 to 3 m into the room for the floor and the ceiling, at the back wall for the wall.
const vec3 ROOM_CEILING = vec3(0.59, 0.57, 0.53), ROOM_WALL = vec3(0.34, 0.32, 0.28);
const vec3 FLOOR_BOARD = vec3(0.18, 0.09, 0.03), FLOOR_TILE = vec3(0.40, 0.38, 0.33), FLOOR_STONE = vec3(0.27, 0.24, 0.21);
const vec3 FLOOR_HOME = vec3(0.29, 0.235, 0.18); // homes: boards and tiles, half each
const float ROOM_DEEP_SHARE = 1.8; // a room's depth over its height: how soon a ray going down or up meets the floor or the ceiling
const float HUNG_COVER = 0.394; // what room_hung covers of a window, on the mean of its picks
const vec3 HUNG_MEAN = vec3(0.515, 0.495, 0.461); // and the mean tone of what covers it
const float HUNG_DAYLIT = 0.776; // room_hung's hung_dim under the mean of its daylight, worked out by hand (log/review.md: B3)
const float LEAST_INTO = 1e-3; // a ray's share into the wall divided by never under this
// A room's independent picks from its lot: fract(lot * k), each k far from a simple ratio of the others (room_hung's too).
const float LOT_HUNG = 13.71, LOT_CLOTH = 7.37, LOT_SIDE = 3.13;
const int CLOTH_KINDS = 7; // the cloths a room may hang (cloth_tone)
// What hangs in a room's window, read here and by room_hung (building_pane.gdshaderinc): data/render/materials.json _shared rooms.
uniform vec4 hung_kinds; // the shares of lots under which a room has curtains, a roller blind, a Venetian blind, a sheer
uniform vec2 curtain_side; // a curtain's width, of the pane: from and over
uniform vec2 roller_down; // a roller blind's drop, of the pane: from and over
uniform float cloth_until[CLOTH_KINDS - 1]; // a cloth's pick: under each share of lots, the cloth of its place
uniform vec3 cloth_tone[CLOTH_KINDS]; // linear
uniform vec4 blind_mean; // a Venetian blind's mean tone and cover over its slats, room_hung's worked out by hand (B3)
uniform vec4 sheer_mean; // a sheer's

// The colour of the cloth a room hangs, by its pick.
vec3 pane_cloth(float h) {
	for (int i = 0; i < CLOTH_KINDS - 1; i++) {
		if (h < cloth_until[i]) return cloth_tone[i];
	}
	return cloth_tone[CLOTH_KINDS - 1];
}

// A room looked into along `V` (into the wall `n`): the back wall square on, the floor or the ceiling more and more as the ray leans.
vec3 room_tone(vec3 V, vec3 n, vec3 floor_tone) {
	float lean = abs(V.y) / max(dot(V, -n), LEAST_INTO) * ROOM_DEEP_SHARE;
	return mix(ROOM_WALL, V.y < 0.0 ? floor_tone : ROOM_CEILING, min(lean, 1.0));
}

// The room `room` with what hangs in its window, each room its own by `lot`, as room_hung picks it.
vec3 pane_tone(float lot, vec3 room) {
	float h1 = fract(lot * LOT_HUNG), h2 = fract(lot * LOT_CLOTH), h3 = fract(lot * LOT_SIDE);
	vec3 cloth = pane_cloth(h2) * HUNG_DAYLIT;
	if (h1 < hung_kinds.x) return mix(room, cloth, min(2.0 * (curtain_side.x + curtain_side.y * h3), 1.0));
	if (h1 < hung_kinds.y) return mix(room, cloth, roller_down.x + roller_down.y * h3);
	if (h1 < hung_kinds.z) return mix(room, blind_mean.rgb, blind_mean.w);
	if (h1 < hung_kinds.w) return mix(room, sheer_mean.rgb, sheer_mean.w);
	return room;
}

// The same, over many rooms under a pixel.
vec3 pane_mean(vec3 room) { return mix(room, HUNG_MEAN, HUNG_COVER); }
