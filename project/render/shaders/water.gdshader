// Water plane at the sim water level: two layers of ripples moving downwind, reflected by Godot's own dielectric model.
// Every number comes from data/render/palette.json "water" (its _doc gives the sources):
// - the ripples are the peak of the fetch-limited wind-sea spectrum (JONSWAP, Hasselmann et al. 1973) and its
//   double frequency, each moving at its own deep-water phase speed (c = g / (2 pi f), so a layer drifts f cells a
//   second when a cell is one wavelength), the way the wind blows (the vegetation material's wind, its single owner);
//   equal in slope, since the f^-5 equilibrium range (Phillips 1958) holds the same mean square slope in every octave;
// - their slope is the mean square slope of a wind-roughened surface (Cox & Munk 1954); what the pixel cannot resolve
//   (a wave needs two pixels: the ripples fade out between each layer's Nyquist limit) goes into the roughness instead,
//   mean square slopes adding up (GGX alpha ~ rms slope, Walter et al. 2007);
// - the reflectance at normal incidence is water's (n = 1.333), given as Godot's SPECULAR (F0 = 0.16 SPECULAR^2), so
//   the sky's reflection follows Fresnel by the renderer's own model.
shader_type spatial;
render_mode cull_disabled;

uniform vec3 shallow : source_color;
uniform vec3 deep : source_color;
uniform float wave_scale; // cells a metre: one over the peak wavelength
uniform float peak_hz; // the peak's frequency: cells a second the main layer drifts
uniform float detail_ratio; // the detail layer's wavelength ratio (frequency ratio squared)
uniform float ripple_strength;
uniform float slope_sigma; // rms slope the ripples stand for, as a roughness when unresolved
uniform float specular_f0;
uniform float fetch_m; // the width of a water body: shallow and deep colours mix in patches this wide
uniform float absorb_per_m; // how fast the deep colour takes over with the depth seen through
uniform float paws_m; // the gust patches that roughen the surface: how wide, and how rough
uniform float paws_rough;
global uniform float wind_x;
global uniform float wind_z;
uniform float waves = 0.3; // the map's weather: how rough, how much foam, how fast the rivers run (render/sky_view.gd)
uniform float foam = 0.4;
uniform float current = 1.2;
uniform sampler2D depth_texture : hint_depth_texture, repeat_disable, filter_nearest;

#include "res://render/shaders/lattice_noise.gdshaderinc"
#include "res://render/shaders/color_space.gdshaderinc"

const float FD_STEP = 0.1; // finite difference, in cells: a tenth of the span the value noise is smooth over

// Slope of the two layers (finite differences of the value noise).
vec2 ripple(vec2 p, float t, float fine) {
	if (fine <= 0.0) return vec2(0.0); // far water: the noise is below a pixel, skip the four reads
	vec2 dir = normalize(vec2(wind_x, wind_z));
	vec2 a = p * wave_scale - dir * peak_hz * t;
	vec2 b = p * wave_scale * detail_ratio - dir * peak_hz * sqrt(detail_ratio) * t;
	float ha = vnoise(a), hb = vnoise(b);
	vec2 d = vec2(vnoise(a + vec2(FD_STEP, 0)) - ha, vnoise(a + vec2(0, FD_STEP)) - ha) + vec2(vnoise(b + vec2(FD_STEP, 0)) - hb, vnoise(b + vec2(0, FD_STEP)) - hb);
	return d / FD_STEP;
}

void fragment() {
	vec3 world = (INV_VIEW_MATRIX * vec4(VERTEX, 1.0)).xyz;
	float pixel = max(fwidth(world.x), fwidth(world.z));
	float fine = 1.0 - smoothstep(0.5 / (wave_scale * detail_ratio), 0.5 / wave_scale, pixel);
	float rough = 0.5 + 2.0 * waves;
	vec2 s = ripple(world.xz - normalize(vec2(wind_x, wind_z)) * current * TIME, TIME, fine) * ripple_strength * fine * rough;
	vec3 n = normalize(vec3(-s.x, 1.0, -s.y));
	NORMAL = normalize((VIEW_MATRIX * vec4(n, 0.0)).xyz);
	// Foam: where the water is shallow over the ground, broken by the ripples' own noise.
	float depth = texture(depth_texture, SCREEN_UV).x;
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	vec3 ndc = vec3(SCREEN_UV, depth) * 2.0 - 1.0;
#else
	vec3 ndc = vec3(SCREEN_UV * 2.0 - 1.0, depth);
#endif
	vec4 under = INV_PROJECTION_MATRIX * vec4(ndc, 1.0);
	float below = length(under.xyz / under.w) - length(VERTEX);
	vec3 c = mix(source_linear(shallow), source_linear(deep), max(vnoise(world.xz / fetch_m), 1.0 - exp(-max(below, 0.0) * absorb_per_m)));
	float edge = (1.0 - smoothstep(0.0, 1.6, below)) * foam;
	float froth = smoothstep(0.35, 0.8, vnoise(world.xz * 0.9 + current * TIME * 0.3) + edge * 0.6);
	c = mix(c, vec3(0.9, 0.93, 0.95), froth * edge);
	ALBEDO = linear_out(c);
	float paws = vnoise((world.xz - normalize(vec2(wind_x, wind_z)) * current * TIME) / paws_m) * vnoise(world.xz / (paws_m * 3.1) + 7.0);
	ROUGHNESS = sqrt(slope_sigma * sqrt(1.0 - fine * fine)) + paws * paws_rough * rough;
	SPECULAR = specular_f0;
}
