// Roadcraft 4.0 buildings (render/city.ts building material): one material for every building surface. COLOR.a is the
// surface kind written by native/src/render/buildings/shell.h: 0 shell face (UV2.x = cooked finish layer + 1, 0 = no
// finish), 1/2 glass pane (dark), 3 far wall with painted windows (UV in bays/storeys, UV2.y = packed window row),
// 4 lamp, 5 lawn. Finishes: world-planar UVs in metres (4.0 shellParts.ts), map normalised by its layer mean, normal
// map, roughness and metal mask. Panes: analytic interior mapping with lights, blinds and curtains (city.ts:282-364).
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
uniform float lit_min = 0.15;
uniform float lit_max = 0.62;
uniform float room_depth_m = 4.5;
uniform float lamp_glow = 4.0;
uniform float window_glow = 1.5;
uniform float day_inside = 0.25;
uniform float glass_reflect = 0.08;
uniform vec3 glass_sky : source_color = vec3(0.45, 0.55, 0.66);
uniform float glass_sky_floor = 0.15;

varying vec3 v_world;
varying vec3 v_wn;

void vertex() {
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_wn = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
}

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

// The finish of a face: albedo tone, perturbed world normal, roughness, metalness.
void finish(int fin, vec3 n, out vec3 tone, out vec3 nn, out float rough, out float metal) {
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

// Weathering of plain finishes (city.ts: macro tone and stains, streaks on walls).
float weather(vec3 n) {
	vec3 t = dot(n.xz, n.xz) < 0.0025 ? vec3(1.0, 0.0, 0.0) : normalize(vec3(n.z, 0.0, -n.x));
	vec2 pm = vec2(dot(v_world, t), v_world.y);
	float macro = vnoise(pm * 0.021) * 0.65 + vnoise(pm * 0.075) * 0.35;
	float streak = abs(n.y) < 0.5 ? smoothstep(0.55, 0.85, vnoise(vec2(pm.x * 0.55, pm.y * 0.02))) : 0.0;
	float stain = 0.3 * (0.18 + 0.12 * vnoise(pm * 0.0275) + (abs(n.y) < 0.5 ? 0.28 * streak : 0.1));
	return (0.9 + 0.2 * macro) * (1.0 - stain);
}

// One window cell (fx, fy in 0..1) of a room w x h metres seen along V: the lit interior, blinds/curtains, the glass.
void window(vec2 f, vec2 size, uint seed, bool dark, vec3 n, vec3 V, out vec3 albedo, out vec3 emit) {
	vec3 t = normalize(vec3(n.z, 0.0, -n.x));
	float rnd = h01(seed * 7919u + 1u), rnd2 = h01(seed * 131u + 17u), sty = h01(seed * 53u + 3u), r3 = h01(seed * 733u + 91u);
	vec3 d = vec3(dot(V, t) / size.x, V.y / size.y, max(dot(V, -n), 0.02) / room_depth_m);
	d.x = abs(d.x) < 1e-4 ? 1e-4 : d.x;
	d.y = abs(d.y) < 1e-4 ? 1e-4 : d.y;
	float ix = (step(0.0, d.x) - f.x) / d.x, iy = (step(0.0, d.y) - f.y) / d.y, iz = 1.0 / d.z;
	float tmin = min(min(ix, iy), iz), hz = clamp(d.z * tmin, 0.0, 1.0);
	vec3 back = mix(vec3(0.62, 0.55, 0.46), vec3(0.50, 0.56, 0.60), rnd);
	vec3 room = iz <= min(ix, iy) ? back : ix < iy ? back * 0.78 : (d.y < 0.0 ? mix(vec3(0.24, 0.17, 0.11), vec3(0.32, 0.30, 0.28), rnd2) : vec3(0.86, 0.85, 0.82));
	room *= 1.0 - 0.45 * hz;
	if (rnd2 >= 0.7 && iz <= min(ix, iy)) room *= 0.55;
	float lit = rnd < mix(lit_min, lit_max, sty) ? 1.0 : 0.0;
	vec3 lamp = rnd2 >= 0.75 ? vec3(0.85, 0.92, 1.0) : vec3(1.0, 0.84, 0.58);
	float cover = 0.0;
	vec3 fabric = vec3(0.0);
	if (r3 < 0.32) {
		float drop = 0.15 + 0.75 * h01(seed * 97u + 5u);
		cover = step(1.0 - drop, f.y) * (0.8 + 0.2 * step(0.5, fract(f.y * 18.0)));
		fabric = mix(vec3(0.78, 0.76, 0.70), vec3(0.60, 0.62, 0.64), h01(seed * 33u + 7u));
	} else if (r3 < 0.5) {
		float wpan = 0.12 + 0.22 * h01(seed * 537u + 9u);
		cover = step(f.x, wpan) + step(1.0 - wpan, f.x);
		fabric = h01(seed * 113u + 11u) >= 0.55 ? vec3(0.64, 0.64, 0.62) : vec3(0.72, 0.66, 0.56);
	}
	vec3 inside = mix(mix(room * day_inside, fabric * day_inside * 3.0, cover), mix(room * lamp * lit * window_glow, fabric * lamp * lit * 0.9, cover), night);
	float cosv = clamp(dot(-V, n), 0.0, 1.0);
	float F = max(0.04 + 0.96 * pow(1.0 - cosv, 5.0), glass_reflect);
	vec3 sky = mix(vec3(0.20, 0.21, 0.22), vec3(0.42, 0.44, 0.46), clamp(0.3 - V.y, 0.0, 1.0)) * (1.0 - 0.92 * night);
	albedo = (vec3(0.03, 0.04, 0.05) + sky * F) * (dark ? 0.6 : 1.0);
	// Daylight sky reflection as light leaving the glass (the compatibility renderer reflects little of the sky).
	vec3 refl = glass_sky * mix(0.75, 1.15, clamp(-V.y + 0.2, 0.0, 1.0)) * (F + glass_sky_floor) * (1.0 - night);
	emit = inside * (dark ? 0.3 : 1.0) * (1.0 - F) + refl * (dark ? 0.7 : 1.0);
}

// Far walls: the window of a bay x storey cell from the packed row (width share, sill, head, flags).
void far_window(vec2 uv, float code, uint seed, vec3 n, vec3 V, vec3 wall, inout vec3 albedo, out vec3 emit, out float glass) {
	emit = vec3(0.0);
	glass = 0.0;
	float ww = mod(code, 64.0) / 63.0, wl = mod(floor(code / 64.0), 64.0) / 63.0, wh = mod(floor(code / 4096.0), 64.0) / 63.0;
	int flags = int(floor(code / 262144.0));
	if ((flags & 4) != 0) return;
	vec2 f = fract(uv), cell = floor(uv);
	bool curtain = (flags & 2) != 0;
	float detail = clamp(1.3 - 2.6 * max(fwidth(uv.x), fwidth(uv.y)), 0.0, 1.0);
	float in_x = step(abs(f.x - 0.5), ww * 0.5), in_y = step(wl, f.y) * step(f.y, wh);
	float share = curtain ? 0.74 : ww * (wh - wl);
	uint cs = seed * 977u + uint(cell.x + 512.0) * 131u + uint(cell.y + 512.0) * 7177u;
	vec3 ga, ge;
	window(vec2((f.x - 0.5) / max(ww, 0.05) + 0.5, (f.y - wl) / max(wh - wl, 0.05)), vec2(3.0, 2.6), cs, h01(cs) < 0.2, n, V, ga, ge);
	float lit_avg = mix(lit_min, lit_max, h01(seed + 3u));
	vec3 avg_e = mix(vec3(0.07, 0.065, 0.06), vec3(1.0, 0.88, 0.64) * lit_avg * 0.42, night);
	float m = in_x * in_y;
	float frame = curtain ? 0.0 : m * (step(abs(f.x - 0.5), ww * 0.5 - 0.04) < 0.5 || f.y < wl + 0.03 || f.y > wh - 0.03 ? 1.0 : 0.0);
	glass = mix(share, m * (1.0 - frame), detail);
	albedo = mix(albedo, mix(vec3(0.05, 0.06, 0.07), ga, detail), glass);
	albedo = mix(albedo, vec3(0.85, 0.84, 0.8), frame * detail);
	emit = mix(avg_e * share, ge * m * (1.0 - frame), detail);
}

void fragment() {
	int kind = int(COLOR.a * 255.0 + 0.5);
	vec3 n = normalize(v_wn);
	vec3 V = normalize(v_world - CAMERA_POSITION_WORLD);
	vec3 albedo = lin(COLOR.rgb), emit = vec3(0.0), nn = n;
	float rough = 0.86, metal = 0.0;
	if (kind == 1 || kind == 2) {
		uint seed = uint(COLOR.r * 255.0 + 0.5) | (uint(COLOR.g * 255.0 + 0.5) << 8u) | (uint(COLOR.b * 255.0 + 0.5) << 16u);
		window(UV, max(UV2, vec2(0.3)), seed, kind == 2, n, V, albedo, emit);
		rough = 0.12;
	} else if (kind == 4) {
		emit = albedo * night * lamp_glow;
	} else if (kind == 5) {
		albedo *= 0.8 + 0.25 * vnoise(v_world.xz * 0.7) + 0.15 * vnoise(v_world.xz * 3.1);
		rough = 0.95;
	} else {
		int fin = int(floor(UV2.x + 0.001)) - 1;
		if (fin >= 0 && fin < 13) {
			vec3 tone;
			finish(fin, n, tone, nn, rough, metal);
			albedo *= tone;
			if (fin != 8 && fin != 7) albedo *= weather(n);
		}
		albedo *= wall_gain;
		if (kind == 3) {
			float glass;
			uint seed = uint(fract(UV2.x) * 256.0 + 0.5);
			far_window(UV, UV2.y, seed, n, V, albedo, albedo, emit, glass);
			rough = mix(rough, 0.15, glass);
			nn = normalize(mix(nn, n, glass));
		}
	}
	ALBEDO = albedo;
	EMISSION = emit;
	ROUGHNESS = rough;
	METALLIC = metal;
	NORMAL = normalize((VIEW_MATRIX * vec4(nn, 0.0)).xyz);
}
