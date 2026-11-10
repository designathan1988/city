// What a room shows through its glass when it is too small to trace (building_pane.gdshaderinc room_trace, room_hung): every level draws
// the same room, or a facade changes colour as the level changes. The tones are room_trace's surfaces under its own shades: the
// daylight's fall at 2 to 3 m into the room for the floor and the ceiling, at the back wall for the wall.
const vec3 ROOM_CEILING = vec3(0.59, 0.57, 0.53), ROOM_WALL = vec3(0.34, 0.32, 0.28);
const vec3 FLOOR_BOARD = vec3(0.18, 0.09, 0.03), FLOOR_TILE = vec3(0.40, 0.38, 0.33), FLOOR_STONE = vec3(0.27, 0.24, 0.21);
const vec3 FLOOR_HOME = vec3(0.29, 0.235, 0.18); // homes: boards and tiles, half each
const float ROOM_DEEP_SHARE = 1.8; // a room's depth over its height: how soon a ray going down or up meets the floor or the ceiling
const float HUNG_COVER = 0.394; // what room_hung covers of a window, on the mean of its picks
const vec3 HUNG_MEAN = vec3(0.515, 0.495, 0.461); // and the mean tone of what covers it
const float HUNG_DIM = 0.776; // room_hung's last 0.8 under the mean of its daylight

// The colour of the cloth a room hangs, by its pick.
vec3 pane_cloth(float h) {
	return h < 0.25 ? vec3(0.86, 0.8, 0.68) : h < 0.45 ? vec3(0.9, 0.89, 0.86) : h < 0.6 ? vec3(0.5, 0.51, 0.53)
			: h < 0.72 ? vec3(0.72, 0.6, 0.45) : h < 0.84 ? vec3(0.4, 0.47, 0.58) : h < 0.93 ? vec3(0.6, 0.34, 0.26) : vec3(0.42, 0.5, 0.38);
}

// A room looked into along `V` (into the wall `n`): the back wall square on, the floor or the ceiling more and more as the ray leans.
vec3 room_tone(vec3 V, vec3 n, vec3 floor_tone) {
	float lean = abs(V.y) / max(dot(V, -n), 1e-3) * ROOM_DEEP_SHARE;
	return mix(ROOM_WALL, V.y < 0.0 ? floor_tone : ROOM_CEILING, min(lean, 1.0));
}

// The room `room` with what hangs in its window, each room its own by `lot`, as room_hung picks it.
vec3 pane_tone(float lot, vec3 room) {
	float h1 = fract(lot * 13.71), h2 = fract(lot * 7.37), h3 = fract(lot * 3.13);
	vec3 cloth = pane_cloth(h2) * HUNG_DIM;
	if (h1 < 0.36) return mix(room, cloth, min(2.0 * (0.14 + 0.2 * h3), 1.0));
	if (h1 < 0.56) return mix(room, cloth, 0.15 + 0.65 * h3);
	if (h1 < 0.68) return mix(room, vec3(0.53, 0.523, 0.496), 0.55);
	if (h1 < 0.8) return mix(room, vec3(0.698, 0.691, 0.667), 0.5);
	return room;
}

// The same, over many rooms under a pixel.
vec3 pane_mean(vec3 room) { return mix(room, HUNG_MEAN, HUNG_COVER); }
