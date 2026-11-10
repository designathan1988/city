// Gradient sky (Roadcraft render/scene.ts makeSky): horizon to zenith, a saturated pale-blue band low over the street
// view, a ground colour below the horizon and the sun's glow. Colours are driven by render/daylight.gd.
shader_type sky;

// render/daylight.gd sets these at every hour, from the first, before the sky is drawn.
uniform vec3 zenith : source_color;
uniform vec3 horizon : source_color;
uniform vec3 low_zenith : source_color;
uniform vec3 low_horizon : source_color;
uniform vec3 ground : source_color;
uniform vec3 sun_color : source_color;
// The low band's share at the eye's height (daylight.gd follow_view works it out, with the air's colour). The shader reads no
// POSITION: a sky that does is drawn again into its radiance map, and that map filtered, at every move of the camera.
uniform float low;
uniform float dark = 0.0; // 1 at night: the street-level sky stays, dimmed
uniform float gain = 1.0;
uniform float sun_disc = 1.0;
// The gradient's own shape: data/game/daylight.json sky_shader (render/daylight.gd), the 4.0's makeSky; parts said here.
uniform float zenith_power; // the horizon gives way to the zenith by the height's power
uniform vec3 sun_warm; // the sun's side of the low band, warmer and brighter
uniform vec2 low_dark; // what night takes of the low band's horizon and of its zenith
uniform vec2 ground_edge; // the ground gives way to the sky between these heights of the eye's direction
uniform vec4 sun_glow; // the glow's power and share, the disc's power and share
uniform vec2 glow_edge; // the glow fades in between these heights

void sky() {
	vec3 dir = normalize(EYEDIR);
	float h = dir.y;
	float up = clamp(h, 0.0, 1.0);
	float sd = LIGHT0_ENABLED ? clamp(dot(dir, LIGHT0_DIRECTION), 0.0, 1.0) : 0.0;
	vec3 col = mix(horizon, zenith, pow(up, zenith_power));
	// The sun's side of the horizon is warmer and brighter.
	vec3 sunward = mix(low_horizon, low_horizon * sun_warm, pow(sd, 3.0) * 0.5) * (1.0 - low_dark.x * dark);
	vec3 low_col = mix(sunward, low_zenith * (1.0 - low_dark.y * dark), smoothstep(0.0, 0.5, up));
	// The reflections read the plain day sky: only the eye at street level sees the deep band.
	if (!AT_CUBEMAP_PASS) col = mix(col, low_col, low);
	col = mix(ground, col, smoothstep(ground_edge.x, ground_edge.y, h));
	float glow = (pow(sd, sun_glow.x) * sun_glow.y + pow(sd, sun_glow.z) * sun_glow.w * sun_disc) * smoothstep(glow_edge.x, glow_edge.y, h);
	COLOR = (col + sun_color * glow) * gain;
}
