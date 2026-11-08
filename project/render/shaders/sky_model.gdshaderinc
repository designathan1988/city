// The clear sky as the air makes it, in the renderer's own light unit: a white diffuse surface square to a DirectionalLight3D of
// energy E shows radiance E (the engine multiplies a directional light's energy by pi and divides its diffuse BRDF by pi:
// servers/rendering/renderer_rd/storage_rd/light_storage.cpp, scene_forward_lights_inc.glsl), so a sun of energy E stands for an
// irradiance pi E, and a radiance written here is what a surface facing it takes as ambient light (the engine's ambient reads the
// sky's radiance map in the same unit). Rayleigh, aerosol (Mie) and ozone, each an exponential layer, after Bruneton & Neyret 2008
// (Precomputed Atmospheric Scattering) and Hillaire 2020 (A Scalable and Production Ready Sky and Atmosphere Rendering Technique):
// single scattering in closed form over the path (each layer's share of the optical depth, Hoffman & Preetham 2002, Rendering
// Outdoor Light Scattering in Real Time), the higher orders as the isotropic sum of a geometric series whose ratio is the share of
// scattered light that scatters again before it leaves (the single-scattering albedo times what a path takes that starts half way
// up the column, where the mean scattering is, and crosses the half left along twice its depth, the mean of a half-space). The path through a layer of scale height H is a homogeneous shell's (Schoenberg 1929): the
// air mass sqrt((x mu)^2 + 2x + 1) - x mu, x = (R + h) / H. render/air.gd sets the values (data/game/daylight.json "air") and
// mirrors sky_sun_at for the sun's own light.
#ifndef SKY_MODEL_INC
#define SKY_MODEL_INC
#include "res://render/shaders/contracts/sky.gdshaderinc"

const float SKY_PI = 3.14159265;
const float SUN_LIMB_DARKENING = 0.6; // the linear limb darkening of the visible disc (Allen's Astrophysical Quantities, 4th ed., 14.7)
const float MOON_RADIUS_RAD = 0.00452; // the moon's mean apparent radius, 15.5 arcminutes (Allen's Astrophysical Quantities, 4th ed., 12.3)
const float LEAST_MU = 0.001; // a ray this close to level still meets the ground or the top of the air

// What render/air.gd hands every shader that draws the sky: the sun's direction (towards it) and its light over the air, the
// Rayleigh betas (per metre, r g b) and scale height, the aerosol's extinction (per metre), single-scattering albedo, phase g and
// scale height, the ozone's betas (per metre at its peak) with the layer's thickness and height folded into its zenith depth, and
// the ground's albedo.
struct Air {
	vec3 sun_dir;
	vec3 sun_top;
	vec4 rayleigh; // beta r g b, scale height m
	vec4 mie; // extinction per m, albedo, g, scale height m
	vec4 ozone; // zenith optical depth r g b, height of the layer m
	vec3 ground;
};

float sky_airmass(float mu, float h, float scale_h) {
	float x = (PLANET_RADIUS_M + h) / scale_h;
	return sqrt(x * x * mu * mu + 2.0 * x + 1.0) - x * mu;
}

// The ozone sits in a thin layer high up: its path is a thin shell's at that height.
float sky_ozone_airmass(float mu, float layer_h) {
	float k = PLANET_RADIUS_M / (PLANET_RADIUS_M + layer_h);
	return 1.0 / sqrt(max(1.0 - k * k * (1.0 - mu * mu), SKY_LEAST_SLANT));
}

// The optical depth from height h along elevation sine mu, to the top of the air or to the ground: Rayleigh (rgb), aerosol
// extinction, ozone (rgb); `ground` is set when the ray meets the ground. Down a slope the density's integral is closed:
// beta H (1 - e^(-h/H)) / -mu.
void sky_depths(Air a, float mu, float h, out vec3 rayleigh, out float mie, out vec3 ozone, out bool ground) {
	ground = mu < 0.0;
	if (ground) {
		float m = max(-mu, LEAST_MU);
		rayleigh = a.rayleigh.rgb * a.rayleigh.w * (1.0 - exp(-h / a.rayleigh.w)) / m;
		mie = a.mie.x * a.mie.w * (1.0 - exp(-h / a.mie.w)) / m;
		ozone = vec3(0.0);
	} else {
		rayleigh = a.rayleigh.rgb * a.rayleigh.w * exp(-h / a.rayleigh.w) * sky_airmass(mu, h, a.rayleigh.w);
		mie = a.mie.x * a.mie.w * exp(-h / a.mie.w) * sky_airmass(mu, h, a.mie.w);
		ozone = a.ozone.rgb * sky_ozone_airmass(mu, a.ozone.w);
	}
}

// The ozone a ray from height h at elevation sine mu crosses, in zenith columns: once from under the layer (a thin shell, its slant
// at the crossing), twice from over it when the ray dips under it (down to its perigee and up again), never from over it otherwise.
float sky_ozone_path(float mu, float h, float layer_h) {
	float k = (PLANET_RADIUS_M + h) / (PLANET_RADIUS_M + layer_h);
	float grazing = k * k * (1.0 - mu * mu);
	float cross = 1.0 / sqrt(max(1.0 - grazing, SKY_LEAST_SLANT));
	if (h <= layer_h) return cross;
	return mu >= 0.0 || grazing >= 1.0 ? 0.0 : 2.0 * cross;
}

// The sun's light after the air from height h, the sun at elevation sine mu over that point's own horizon (rgb, the light's unit),
// with the earth's shadow at that height: the sun sets behind the horizon's dip, -sqrt(2h / R), across its own disc.
vec3 sky_sun_from(Air a, float h, float mu) {
	vec3 r = a.rayleigh.rgb * a.rayleigh.w * exp(-h / a.rayleigh.w) * sky_airmass(mu, h, a.rayleigh.w);
	float m = a.mie.x * a.mie.w * exp(-h / a.mie.w) * sky_airmass(mu, h, a.mie.w);
	vec3 o = a.ozone.rgb * sky_ozone_path(mu, h, a.ozone.w);
	float dip = -sqrt(2.0 * max(h, 1.0) / PLANET_RADIUS_M);
	return a.sun_top * exp(-(r + m + o)) * smoothstep(dip - SUN_RADIUS_RAD, dip + SUN_RADIUS_RAD, mu);
}

// The sun's light after the air from height h over the eye (render/air.gd mirrors it at the ground).
vec3 sky_sun_at(Air a, float h) {
	return sky_sun_from(a, h, a.sun_dir.y);
}

// The sun's light on a layer of scale height `scale_h` seen along `dir` from the eye. The layer scatters most at its scale height over
// the point under it, `reach` along the ground (scale_h over the view's slope, at most the layer's own horizon, sqrt(2 R H); by the
// eye for a view down), where the sun stands higher by the arc reach / R towards it and lower away from it. Once the sun is under
// that point's horizon (depression d), the earth's shadow fills the layer up to R (1 / cos d - 1) over it, and the share above the
// shadow, exp(-h_s / H), is lit by the sun's light at h_s + H, the middle of the lit part: its rays graze the air and the ozone
// layer, which take the red and the orange (Chappuis), so the dusk's sky turns blue as its lit part rises. With the sun up there it
// is the light at H.
vec3 sky_layer_lit(Air a, vec3 dir, float scale_h) {
	float horizon = sqrt(2.0 * PLANET_RADIUS_M * scale_h);
	float reach = dir.y <= 0.0 ? 0.0 : min(scale_h * sqrt(1.0 - dir.y * dir.y) / max(dir.y, LEAST_MU), horizon);
	vec2 view = dir.xz, sun = a.sun_dir.xz;
	float facing = length(view) > LEAST_MU && length(sun) > LEAST_MU ? dot(normalize(view), normalize(sun)) : 0.0;
	float elev = asin(clamp(a.sun_dir.y, -1.0, 1.0)) + reach / PLANET_RADIUS_M * facing;
	float shadow = elev < 0.0 ? PLANET_RADIUS_M * (1.0 / cos(elev) - 1.0) : 0.0;
	return sky_sun_from(a, shadow + scale_h, sin(elev)) * exp(-shadow / scale_h);
}

float sky_rayleigh_phase(float c) {
	return 3.0 / (16.0 * SKY_PI) * (1.0 + c * c);
}

float sky_mie_phase(float g, float c) { // Henyey-Greenstein
	float d = 1.0 + g * g - 2.0 * g * c;
	return (1.0 - g * g) / (4.0 * SKY_PI * d * sqrt(d));
}

// The light the air sends along `dir` towards an eye at height h, from the sun lit over it: what the path scatters in.
// The sun's light at the scattering is each layer's where it scatters most along the view (sky_layer_lit).
vec3 sky_inscatter(Air a, vec3 dir, float h, out vec3 through, out bool ground) {
	vec3 tr;
	float tm;
	vec3 to;
	sky_depths(a, dir.y, h, tr, tm, to, ground);
	vec3 ext = tr + vec3(tm) + to;
	through = exp(-ext);
	float c = dot(dir, a.sun_dir);
	vec3 share = (vec3(1.0) - through) / max(ext, vec3(1e-6));
	vec3 lit = sky_layer_lit(a, dir, a.rayleigh.w);
	vec3 lit_mie = sky_layer_lit(a, dir, a.mie.w);
	vec3 single = SKY_PI * (lit * tr * sky_rayleigh_phase(c) + lit_mie * tm * a.mie.y * sky_mie_phase(a.mie.z, c)) * share;
	// The higher orders: isotropic, their ratio from the zenith column: half of it, along a mean path of twice its depth.
	vec3 zr = a.rayleigh.rgb * a.rayleigh.w;
	float zm = a.mie.x * a.mie.w;
	vec3 zext = zr + vec3(zm) + a.ozone.rgb;
	vec3 again = (zr + vec3(zm * a.mie.y)) / zext * (vec3(1.0) - exp(-zext));
	vec3 iso = SKY_PI * (lit * tr + lit_mie * tm * a.mie.y) / (4.0 * SKY_PI) * share;
	return single + iso * again / (vec3(1.0) - again);
}

// The sky's radiance along `dir` from height h, the ground's below the horizon (its albedo under the sun and the zenith's light,
// through the air between): the clear sky the radiance map, the ambient light and the aerial perspective all read.
vec3 sky_radiance(Air a, vec3 dir, float h) {
	vec3 through;
	bool ground;
	vec3 sky = sky_inscatter(a, dir, h, through, ground);
	if (ground) {
		vec3 t_up;
		bool g_up;
		vec3 zenith = sky_inscatter(a, vec3(0.0, 1.0, 0.0), 0.0, t_up, g_up);
		vec3 lit = a.ground * (sky_sun_at(a, 0.0) * max(a.sun_dir.y, 0.0) + zenith);
		sky += lit * through;
	}
	return sky;
}

// The sun's disc, limb darkened: its light over its solid angle (pi r^2), as radiance. For the picture only: the radiance map
// leaves it out, the sun's own light already lights and glints.
vec3 sky_sun_disc(Air a, vec3 dir, float h) {
	float angle = acos(clamp(dot(dir, a.sun_dir), -1.0, 1.0));
	if (angle >= SUN_RADIUS_RAD) return vec3(0.0);
	float r = angle / SUN_RADIUS_RAD;
	float limb = 1.0 - SUN_LIMB_DARKENING * (1.0 - sqrt(1.0 - r * r));
	return sky_sun_at(a, h) * limb / (SUN_RADIUS_RAD * SUN_RADIUS_RAD) / (1.0 - SUN_LIMB_DARKENING / 3.0);
}

// What the hour and the weather lay over the clear air (render/daylight.gd): the moon (towards it, its light over the air; none while
// the sun is up, whose sky outshines it 10^5 times), the town's light sent up (the radiance of the ground under the air, lit by its
// lamps) and the share of it an overcast deck sends back down, the deck's cover and the light it lets down, a bolt's flash.
struct SkyWeather {
	vec3 moon_dir;
	vec3 moon_top;
	vec3 city_up;
	float cloud_reflect;
	float overcast;
	float overcast_pass;
	float flash;
};

// The sky lit by one light over the air (a's sun: the sun or the moon) along `dir` from height h: the clear air, an overcast deck
// over it as the weather has it (the CIE overcast sky, CIE S 011/E:2003 type 1: L = Lz (1 + 2 sin(elevation)) / 3, its level light
// E = 7/9 pi Lz a share of the clear sky's direct light on level ground).
vec3 sky_lit(Air a, SkyWeather w, vec3 dir, float h) {
	vec3 col = sky_radiance(a, dir, h);
	float lz = 9.0 / 7.0 * w.overcast_pass * dot(sky_sun_at(a, 0.0), vec3(0.2126, 0.7152, 0.0722)) * max(a.sun_dir.y, 0.0);
	return mix(col, vec3(lz * (1.0 + 2.0 * max(dir.y, 0.0)) / 3.0), w.overcast);
}

// The town's own light in its sky: the ground under the air at the radiance city_up, scattered towards the eye along the view, each
// layer's share of the path as for the sun (sky_inscatter), with its phase taken over the lower hemisphere the light comes from.
// Rayleigh's phase is symmetric fore and aft, so half of it lies in any half-space; the aerosol's leans forward, half of it at a
// level view and only its back hemisphere looking straight up, B(g) = (1 - g) / (2 g) ((1 + g) / sqrt(1 + g^2) - 1), taken as linear
// in the sine between. Below the horizon the town itself. An overcast deck sends cloud_reflect of the town's light back, evenly.
vec3 sky_glow(Air a, SkyWeather w, vec3 dir, float h) {
	vec3 tr;
	float tm;
	vec3 to;
	bool ground;
	sky_depths(a, dir.y, h, tr, tm, to, ground);
	vec3 ext = tr + vec3(tm) + to;
	vec3 through = exp(-ext);
	vec3 share = (vec3(1.0) - through) / max(ext, vec3(1e-6));
	float g = a.mie.z;
	float back = (1.0 - g) / (2.0 * g) * ((1.0 + g) / sqrt(1.0 + g * g) - 1.0);
	float mie_half = mix(0.5, back, clamp(dir.y, 0.0, 1.0));
	vec3 col = w.city_up * (0.5 * tr + vec3(tm * a.mie.y * mie_half)) * share;
	if (ground) col += w.city_up * through;
	return mix(col, w.city_up * w.cloud_reflect, w.overcast);
}

// The sky as seen along `dir` from height h: the sun's, the moon's, the town's light in it, a bolt's light. The discs are the
// picture's own (sky_sun_disc, sky_moon_disc).
vec3 sky_seen(Air a, SkyWeather w, vec3 dir, float h) {
	vec3 col = sky_lit(a, w, dir, h);
	if (max(w.moon_top.r, max(w.moon_top.g, w.moon_top.b)) > 0.0) {
		Air m = a;
		m.sun_dir = w.moon_dir;
		m.sun_top = w.moon_top;
		col += sky_lit(m, w, dir, h);
	}
	return (col + sky_glow(a, w, dir, h)) * (1.0 + w.flash);
}

// The full moon's disc: its light after the air over its solid angle, as radiance (as sky_sun_disc), even across it (a full moon
// shows next to no limb darkening).
vec3 sky_moon_disc(Air a, SkyWeather w, vec3 dir, float h) {
	if (dot(dir, w.moon_dir) < cos(MOON_RADIUS_RAD)) return vec3(0.0);
	Air m = a;
	m.sun_dir = w.moon_dir;
	m.sun_top = w.moon_top;
	return sky_sun_at(m, h) / (MOON_RADIUS_RAD * MOON_RADIUS_RAD);
}

// The irradiance that sky gives level ground, over pi (the radiance of a white diffuse floor under it): a cosine-weighted mean of the
// upper hemisphere over a fixed set of directions. Worked once a change of the hour or the weather (render/sky_light_probe.gd), never
// once a pixel.
vec3 sky_level_seen(Air a, SkyWeather w, float h) {
	const int RINGS = 8, AROUND = 16;
	vec3 sum = vec3(0.0);
	float weight = 0.0;
	for (int i = 0; i < RINGS; i++) {
		float mu = (float(i) + 0.5) / float(RINGS); // equal steps of sin(elevation)
		float across = sqrt(1.0 - mu * mu);
		for (int k = 0; k < AROUND; k++) {
			float az = (float(k) + 0.5) * 2.0 * SKY_PI / float(AROUND);
			sum += sky_seen(a, w, vec3(across * cos(az), mu, across * sin(az)), h) * mu;
			weight += mu;
		}
	}
	return sum / weight;
}

#endif
