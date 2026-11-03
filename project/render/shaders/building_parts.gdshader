// Instanced facade parts (native/src/render/buildings/shell.h BoxInstance): frames, sills, railings, shutters, brise
// fins, roof rails and fence panels drawn as unit-cube MultiMesh instances. INSTANCE_CUSTOM = (sRGB tone rgb, finish
// layer); the geometry carries no finish vertex attribute, so this is building.gdshader with the finish and tone read
// from the instance instead of COLOR/UV2.
shader_type spatial;
render_mode cull_back;

global uniform float night;

uniform sampler2DArray finish_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2DArray finish_normal : filter_linear_mipmap, repeat_enable;
uniform sampler2DArray finish_rough : filter_linear_mipmap, repeat_enable;
uniform float finish_m[13];
uniform float finish_mean[13];
uniform float finish_normal_k[13];
uniform float finish_metal[13];
uniform float wall_gain = 1.0;

varying vec3 v_world;
varying vec3 v_wn;
varying flat vec4 v_custom;

uint pcg(uint v) {
	uint s = v * 747796405u + 2891336453u;
	uint w = ((s >> ((s >> 28u) + 4u)) ^ s) * 277803737u;
	return (w >> 22u) ^ w;
}
float h01(uint v) { return float(pcg(v)) / 4294967295.0; }
vec3 lin(vec3 c) { return pow(c, vec3(2.2)); }

float vnoise(vec2 p) {
	vec2 i = floor(p), f = fract(p);
	f = f * f * (3.0 - 2.0 * f);
	uvec2 u = uvec2(ivec2(i) + 32768);
	float a = h01(u.x * 73856093u ^ u.y * 19349663u), b = h01((u.x + 1u) * 73856093u ^ u.y * 19349663u);
	float c = h01(u.x * 73856093u ^ (u.y + 1u) * 19349663u), d = h01((u.x + 1u) * 73856093u ^ (u.y + 1u) * 19349663u);
	return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

void finish_layer(int fin, vec3 n, out vec3 tone, out vec3 nn, out float rough, out float metal) {
	vec3 t = dot(n.xz, n.xz) < 0.0025 ? vec3(1.0, 0.0, 0.0) : normalize(vec3(n.z, 0.0, -n.x));
	vec3 b = cross(n, t);
	vec2 uv = vec2(dot(v_world, t), dot(v_world, b)) / finish_m[fin];
	vec3 layer = vec3(uv, float(fin));
	vec4 a = texture(finish_map, layer);
	vec3 nm = texture(finish_normal, layer).rgb * 2.0 - 1.0;
	vec4 r = texture(finish_rough, layer);
	tone = min(a.rgb / finish_mean[fin], vec3(2.0));
	float k = finish_normal_k[fin];
	nn = normalize(t * nm.x * k + b * nm.y * k + n * max(nm.z, 0.2));
	rough = r.g;
	metal = finish_metal[fin] * (fin == 8 ? r.b : 1.0);
}

float weather(vec3 n) {
	vec3 t = dot(n.xz, n.xz) < 0.0025 ? vec3(1.0, 0.0, 0.0) : normalize(vec3(n.z, 0.0, -n.x));
	vec2 pm = vec2(dot(v_world, t), v_world.y);
	float macro = vnoise(pm * 0.021) * 0.65 + vnoise(pm * 0.075) * 0.35;
	float streak = abs(n.y) < 0.5 ? smoothstep(0.55, 0.85, vnoise(vec2(pm.x * 0.55, pm.y * 0.02))) : 0.0;
	float stain = 0.3 * (0.18 + 0.12 * vnoise(pm * 0.0275) + (abs(n.y) < 0.5 ? 0.28 * streak : 0.1));
	return (0.9 + 0.2 * macro) * (1.0 - stain);
}

void vertex() {
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_wn = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
	v_custom = INSTANCE_CUSTOM;
}

void fragment() {
	vec3 n = normalize(v_wn);
	vec3 albedo = lin(v_custom.rgb), nn = n;
	float rough = 0.86, metal = 0.0;
	int fin = int(v_custom.a + 0.5);
	if (fin >= 0 && fin < 13) {
		vec3 tone;
		finish_layer(fin, n, tone, nn, rough, metal);
		albedo *= tone;
		if (fin != 8 && fin != 7) albedo *= weather(n);
	}
	ALBEDO = albedo * wall_gain;
	ROUGHNESS = rough;
	METALLIC = metal;
	NORMAL = normalize((VIEW_MATRIX * vec4(nn, 0.0)).xyz);
}
