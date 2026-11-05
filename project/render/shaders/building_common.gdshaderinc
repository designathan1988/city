// Helpers shared by building.gdshader and building_parts.gdshader: hash, noise, finish sampling and weathering.
// Included after the uniforms and varyings they use.

#include "res://render/shaders/building_hash.gdshaderinc"

// The lattice's values, one a texel (tools/assets/make_lattice_noise.py): value noise is the sampler's bilinear filter at a point
// moved by the smooth step, one fetch where four hashes ran on every pixel of every wall.
uniform sampler2D noise_lattice : filter_linear, repeat_enable;

float vnoise(vec2 p) {
	vec2 i = floor(p), f = fract(p);
	f = f * f * (3.0 - 2.0 * f);
	return textureLod(noise_lattice, (i + f + 0.5) / vec2(textureSize(noise_lattice, 0)), 0.0).r;
}

// The finish of a face: albedo tone, perturbed world normal, roughness, metalness.
void finish_layer(int fin, vec3 n, out vec3 tone, out vec3 nn, out float rough, out float metal) {
	vec3 t = dot(n.xz, n.xz) < 0.0025 ? vec3(1.0, 0.0, 0.0) : normalize(vec3(n.z, 0.0, -n.x));
	vec3 b = cross(n, t);
	vec2 uv = vec2(dot(v_world, t), dot(v_world, b)) / finish_m[fin];
	vec3 layer = vec3(uv, float(fin));
	vec4 a = texture(finish_map, layer);
	vec3 nm = texture(finish_normal, layer).rgb * 2.0 - 1.0;
	vec4 r = texture(finish_rough, layer);
	tone = a.rgb;
	float k = finish_normal_k[fin];
	nn = normalize(t * nm.x * k + b * nm.y * k + n * max(nm.z, 0.2));
	rough = r.g;
	metal = finish_metal[fin] * (fin == 8 ? r.b : 1.0);
}

// Weathering of plain finishes: a macro tone and stains, streaks down the walls.
float weather(vec3 n) {
	vec3 t = dot(n.xz, n.xz) < 0.0025 ? vec3(1.0, 0.0, 0.0) : normalize(vec3(n.z, 0.0, -n.x));
	vec2 pm = vec2(dot(v_world, t), v_world.y);
	float macro = vnoise(pm * 0.021) * 0.65 + vnoise(pm * 0.075) * 0.35;
	float streak = abs(n.y) < 0.5 ? smoothstep(0.55, 0.85, vnoise(vec2(pm.x * 0.55, pm.y * 0.02))) : 0.0;
	float stain = 0.3 * (0.18 + 0.12 * vnoise(pm * 0.0275) + (abs(n.y) < 0.5 ? 0.28 * streak : 0.1));
	return (0.9 + 0.2 * macro) * (1.0 - stain);
}
