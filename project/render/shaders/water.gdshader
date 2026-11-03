// Water plane at the sim water level (Roadcraft render/terrain.ts water material, without the depth term): two
// scrolling ripple normals from value noise, a Fresnel blend from the water colour to the sky colour, sparse
// white caps far out. Colours from data/render/palette.json "water".
shader_type spatial;
render_mode cull_disabled;

uniform vec3 shallow : source_color;
uniform vec3 deep : source_color;
uniform vec3 sky_tint : source_color = vec3(0.435, 0.584, 0.722);
uniform float wave_scale = 0.073;
uniform float wave_speed = 0.035;
uniform float ripple_strength = 0.35;

float hash(vec2 p) {
	return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
	vec2 i = floor(p);
	vec2 f = fract(p);
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(mix(hash(i), hash(i + vec2(1, 0)), u.x), mix(hash(i + vec2(0, 1)), hash(i + vec2(1, 1)), u.x), u.y);
}

// Slope of two noise layers scrolling in different directions (finite differences).
vec2 ripple(vec2 p, float t) {
	vec2 a = p * wave_scale + vec2(0.035, 0.021) * t * wave_speed * 30.0;
	vec2 b = p * wave_scale * 3.1 + vec2(-0.026, 0.047) * t * wave_speed * 30.0;
	float e = 0.1;
	float ha = noise(a), hb = noise(b);
	vec2 d = vec2(noise(a + vec2(e, 0)) - ha, noise(a + vec2(0, e)) - ha) * 0.62 + vec2(noise(b + vec2(e, 0)) - hb, noise(b + vec2(0, e)) - hb) * 0.48;
	return d / e;
}

void fragment() {
	vec3 world = (INV_VIEW_MATRIX * vec4(VERTEX, 1.0)).xyz;
	float fine = 1.0 - smoothstep(0.4, 1.6, max(fwidth(world.x), fwidth(world.z)));
	vec2 s = ripple(world.xz, TIME) * ripple_strength * fine;
	vec3 n = normalize(vec3(-s.x, 1.0, -s.y));
	NORMAL = normalize((VIEW_MATRIX * vec4(n, 0.0)).xyz);
	vec3 v = normalize((INV_VIEW_MATRIX * vec4(VIEW, 0.0)).xyz);
	float f = 0.06 + 0.94 * pow(1.0 - clamp(dot(v, n), 0.0, 1.0), 3.0);
	vec3 c = mix(shallow, deep, 0.5 + 0.5 * noise(world.xz * 0.0031 + TIME * 0.0011));
	c = mix(c * (1.0 - 0.6 * f), sky_tint, 0.35 * f);
	ALBEDO = c;
	ROUGHNESS = 0.15;
	SPECULAR = 0.6;
}
