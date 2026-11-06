// A material of a signature building (tools/assets/cook_signatures.py): a colour, a picture over it, how it reflects. Both signature shaders read it.
#include "res://render/shaders/building_hash.gdshaderinc"
#include "res://render/shaders/building_lamps.gdshaderinc"

uniform vec4 albedo : source_color = vec4(1.0);
uniform sampler2D picture : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform bool pictured = false;
uniform float roughness = 0.8;
uniform float metallic = 0.0;
uniform vec3 emission : source_color = vec3(0.0);
uniform float alpha_cut = 0.0;
uniform bool two_sided = false;
uniform bool windows = false; // all of it glass, its uv in metres with one cell a pane (cook_signatures.py rooms_in_uv), or
uniform bool painted_windows = false; // the alpha of its picture says what is glass, one bay and one storey to the cell

global uniform float night;
global uniform float rooms_awake; // the share of people awake this hour (daylight.gd)
global uniform float building_night_windows; // the Effects panel: 0 puts the lit rooms out

const float GLASS_CELL_M = 3.2; // cook_signatures.py ROOM_M
const float ROOM_GLOW = 0.62; // window_glow of the plain buildings (materials.json)
const float ROOMS_IN_USE = 0.64; // of those awake, the rooms with the lamp on: building_pane.gdshaderinc pane_lamp

// What the room behind a cell gives at night, by the rule of the plain buildings (pane_lamp), with an office's lamps.
vec3 room_light(vec2 uv, float glass) {
	ivec2 cell = ivec2(floor(painted_windows ? uv : uv / GLASS_CELL_M));
	uint slot = uint(cell.x) * 73856093u ^ uint(cell.y) * 19349663u;
	float lot = h01(slot);
	float level = 0.3 + 0.85 * fract(lot * 7.13) * fract(lot * 2.17 + 0.4);
	float lit = step(h01(slot + 977u), rooms_awake * ROOMS_IN_USE) * level * building_night_windows;
	return room_lamp(fract(lot * 17.31), 1.0) * (lit * night * ROOM_GLOW * glass);
}

vec4 look_colour(vec2 uv) {
	return pictured ? albedo * texture(picture, uv) : albedo;
}
