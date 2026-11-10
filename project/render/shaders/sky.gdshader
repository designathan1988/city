// Gradient sky (Roadcraft render/scene.ts makeSky): horizon to zenith, a saturated pale-blue band low over the street
// view, a ground colour below the horizon and the sun's glow. Colours are driven by render/daylight.gd.
shader_type sky;

uniform vec3 zenith : source_color = vec3(0.302, 0.498, 0.769);
uniform vec3 horizon : source_color = vec3(0.788, 0.863, 0.918);
uniform vec3 low_zenith : source_color = vec3(0.165, 0.4, 0.784);
uniform vec3 low_horizon : source_color = vec3(0.525, 0.741, 0.941);
uniform vec3 ground : source_color = vec3(0.435, 0.478, 0.408);
uniform vec3 sun_color : source_color = vec3(1.0, 0.941, 0.824);
uniform float low = 0.5;
uniform float low_from_m;
uniform float low_to_m;
uniform float dark = 0.0; // 1 at night: the street-level sky stays, dimmed
uniform float gain = 1.0;
uniform float sun_disc = 1.0;

void sky() {
	vec3 dir = normalize(EYEDIR);
	float h = dir.y;
	float up = clamp(h, 0.0, 1.0);
	float sd = LIGHT0_ENABLED ? clamp(dot(dir, LIGHT0_DIRECTION), 0.0, 1.0) : 0.0;
	vec3 col = mix(horizon, zenith, pow(up, 0.62));
	// The sun's side of the horizon is warmer and brighter.
	vec3 sunward = mix(low_horizon, low_horizon * vec3(1.12, 1.04, 0.92), pow(sd, 3.0) * 0.5) * (1.0 - 0.85 * dark);
	vec3 low_col = mix(sunward, low_zenith * (1.0 - 0.7 * dark), smoothstep(0.0, 0.5, up));
	// The reflections read the plain day sky: only the eye at street level sees the deep band.
	if (!AT_CUBEMAP_PASS) col = mix(col, low_col, low * (1.0 - smoothstep(low_from_m, low_to_m, POSITION.y)));
	col = mix(ground, col, smoothstep(-0.12, 0.02, h));
	float glow = (pow(sd, 7.0) * 0.28 + pow(sd, 120.0) * 0.9 * sun_disc) * smoothstep(-0.05, 0.05, h);
	COLOR = (col + sun_color * glow) * gain;
}
