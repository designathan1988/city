// Far/HLOD building masses (native/src/render/buildings/building_renderer.cpp, superchunk level): the cheap sibling of
// building.gdshader. No interior mapping, no finish textures and no vertex UV2 — the colour comes from the vertex only
// and the windows are a flat darkened sky, so the shell can drop UV2 (D4) and the material stays one draw per superchunk.
shader_type spatial;
render_mode cull_back;

global uniform float night;

uniform vec3 glass_sky : source_color = vec3(0.45, 0.55, 0.66);
uniform float glass_floor = 0.18;
uniform float wall_gain = 1.0;

varying vec3 v_world;
varying vec3 v_wn;

// Window mask of a far wall cell: the painted-window row is gone with UV2, so a fixed share stands in for it.
const float WIN_W = 0.45;
const float WIN_SILL = 0.35;
const float WIN_HEAD = 0.80;

vec3 to_linear(vec3 c) {
	return mix(c / 12.92, pow((c + 0.055) / 1.055, vec3(2.4)), step(0.04045, c));
}

void vertex() {
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_wn = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
}

void fragment() {
	int kind = int(COLOR.a * 255.0 + 0.5);
	vec3 n = normalize(v_wn);
	vec3 albedo = OUTPUT_IS_SRGB ? COLOR.rgb : to_linear(COLOR.rgb);
	vec3 glass = mix(vec3(0.05, 0.06, 0.07), glass_sky, glass_floor) * (1.0 - 0.92 * night);
	vec3 emit = vec3(0.0);
	float rough = 0.9, metal = 0.0;
	if (kind == 1 || kind == 2) {
		albedo = glass;
		rough = 0.2;
	}
	if (kind == 3) {
		vec2 f = fract(UV);
		float m = step(abs(f.x - 0.5), WIN_W * 0.5) * step(WIN_SILL, f.y) * step(f.y, WIN_HEAD);
		albedo = mix(albedo, glass, m);
		rough = mix(rough, 0.35, m);
	}
	albedo *= wall_gain;
	if (kind == 4) emit = albedo * night;
	ALBEDO = albedo;
	EMISSION = emit;
	ROUGHNESS = rough;
	METALLIC = metal;
	NORMAL = normalize((VIEW_MATRIX * vec4(n, 0.0)).xyz);
}
