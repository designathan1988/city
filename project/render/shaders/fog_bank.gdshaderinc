// The banks of painted fog for atmosphere.gdshader: the slab a ray crosses, read from the fog window the core paints
// (render/sky_view.gd sets the map, the slab, the window and the wind every frame), broken into wisps the wind carries; their
// light is cloud_light.gdshaderinc's. Its look is data/game/sky.json atmosphere.

uniform sampler2D fog_map : repeat_enable, filter_linear;
uniform float fog_density = 1.0;
uniform vec2 fog_slab = vec2(0.0);
uniform vec3 fog_window = vec3(0.0, 0.0, 1.0); // low corner x, z and side
uniform float fog_extinction;
uniform vec2 wind_dir; // set every frame
// Lengths in metres; each vector's parts are said here.
uniform vec4 fog_billow; // a bank's billows: the wide ones across and the fine ones (m), how much flatter each is up
uniform vec4 fog_octaves; // the wide billows' share, their second octave's share, the fine ones' share, the octave's step
uniform float fog_reach_m; // the farthest a ray marches the fog
uniform float fog_flow_s; // the flow's period
uniform float fog_least; // a texel thinner than this is clear air
uniform float fog_below_m; // the fog reaches this far under the ground its texel keeps
uniform float fog_least_thick; // the least thickness its height thins over (m)
uniform float fog_drift; // how far the wind carries the wisps over a flow's period, of the wind
uniform vec4 fog_wisp; // a wisp's edges in the noise; its noise taken from and over the thin share
uniform float fog_done; // a ray this hidden stops marching

const int FOG_STEPS = 36; // the grain of the start goes with the length of a step: 2.1 grey levels at 24, measured on a bank
// Inigo Quilez's 3D hash: p = fract(p / pi + 0.1) * 17, then the product of its parts by their sum.
const float FOG_HASH_K = 0.3183099, FOG_HASH_ADD = 0.1, FOG_HASH_MUL = 17.0;
const float FOG_OCTAVE_SHIFT = 4.1; // the second octave read away from the first (any offset off the lattice)
const vec3 FLOW_SHIFT = vec3(15.0, 0.0, 37.0); // the flow's second phase read away from the first
const float LEAST_DIP = 1e-4; // a ray flatter than this runs along the fog's slab
const float WINDOW_SLACK = 0.001; // of the fog window's side: the margin of the step range over float rounding (fog_amount)

// Value noise by hash, no tile to repeat: a wide bank reads no lattice.
float fog_hash(vec3 p) {
	p = fract(p * FOG_HASH_K + FOG_HASH_ADD);
	p *= FOG_HASH_MUL;
	return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float fog_value(vec3 x) {
	vec3 i = floor(x);
	vec3 f = fract(x);
	f = f * f * (3.0 - 2.0 * f);
	return mix(
		mix(mix(fog_hash(i), fog_hash(i + vec3(1.0, 0.0, 0.0)), f.x), mix(fog_hash(i + vec3(0.0, 1.0, 0.0)), fog_hash(i + vec3(1.0, 1.0, 0.0)), f.x), f.y),
		mix(mix(fog_hash(i + vec3(0.0, 0.0, 1.0)), fog_hash(i + vec3(1.0, 0.0, 1.0)), f.x), mix(fog_hash(i + vec3(0.0, 1.0, 1.0)), fog_hash(i + vec3(1.0, 1.0, 1.0)), f.x), f.y),
		f.z);
}

// The billows of a fog bank, wide and fine (fog_billow), so the bank breaks into wisps with the ground showing between them.
float fog_noise(vec3 q) {
	vec3 wide = vec3(q.x, q.y * fog_billow.z, q.z) / fog_billow.x;
	vec3 fine = vec3(q.x, q.y * fog_billow.w, q.z) / fog_billow.y;
	return fog_value(wide) * fog_octaves.x + fog_value(wide * fog_octaves.w + FOG_OCTAVE_SHIFT) * fog_octaves.y + fog_value(fine) * fog_octaves.z;
}

// The painted fog along the ray: the slab it fills, read from its window; thickness thinning with height over the ground
// the texel keeps, broken into wisps the wind carries (noise_volume moved by a flow of two phases, Vlachos 2010).
float fog_amount(vec3 ro, vec3 rd, float t_scene, float jitter, float time) {
	float t0 = 0.0, t1 = min(t_scene, fog_reach_m);
	if (abs(rd.y) > LEAST_DIP) {
		float ta = (fog_slab.x - ro.y) / rd.y, tb = (fog_slab.y - ro.y) / rd.y;
		t0 = max(t0, min(ta, tb));
		t1 = min(t1, max(ta, tb));
	} else if (ro.y < fog_slab.x || ro.y > fog_slab.y) {
		return 0.0;
	}
	if (t1 <= t0) return 0.0;
	float step_len = (t1 - t0) / float(FOG_STEPS);
	// The steps keep their places (t0, step_len: the grain is tuned on them); only those that cannot fall inside the window
	// are not visited. The ray's stretch over the window in x and z is taken on the window widened by WINDOW_SLACK of its
	// side, far beyond float rounding, and one step more each way: every step the test in the loop keeps is in the range.
	float tw0 = t0, tw1 = t1;
	for (int k = 0; k < 2; k++) {
		float o = k == 0 ? ro.x : ro.z;
		float d = k == 0 ? rd.x : rd.z;
		float lo = (k == 0 ? fog_window.x : fog_window.y) - WINDOW_SLACK * fog_window.z;
		float hi = lo + (1.0 + 2.0 * WINDOW_SLACK) * fog_window.z;
		if (d == 0.0) {
			if (o < lo || o > hi) return 0.0; // every step stands at this x (or z), outside the window
			continue;
		}
		float ta = (lo - o) / d, tb = (hi - o) / d;
		tw0 = max(tw0, min(ta, tb));
		tw1 = min(tw1, max(ta, tb));
	}
	if (tw1 < tw0) return 0.0;
	int i_first = int(clamp(floor((tw0 - t0) / step_len - jitter) - 1.0, 0.0, float(FOG_STEPS)));
	int i_last = int(clamp(ceil((tw1 - t0) / step_len - jitter) + 1.0, -1.0, float(FOG_STEPS - 1)));
	float keep = 1.0;
	float phase_a = fract(time / fog_flow_s), phase_b = fract(time / fog_flow_s + 0.5), blend_b = abs(phase_a - 0.5) * 2.0;
	for (int i = i_first; i <= i_last; i++) {
		vec3 q = ro + rd * (t0 + (float(i) + jitter) * step_len);
		vec2 local = (q.xz - fog_window.xy) / fog_window.z;
		if (local.x < 0.0 || local.y < 0.0 || local.x > 1.0 || local.y > 1.0) continue;
		vec4 m = texture(fog_map, q.xz / fog_window.z);
		if (m.r < fog_least) continue;
		float over = q.y - m.g;
		if (over < -fog_below_m) continue;
		float thin = exp(-max(over, 0.0) / max(m.b, fog_least_thick));
		vec2 flow = wind_dir * m.a * fog_drift;
		float na = fog_noise(q - vec3(flow.x, 0.0, flow.y) * phase_a);
		float nb = fog_noise(q - vec3(flow.x, 0.0, flow.y) * phase_b + FLOW_SHIFT);
		float wisp = smoothstep(fog_wisp.x, fog_wisp.y, mix(na, nb, blend_b) * (fog_wisp.z + fog_wisp.w * thin));
		keep *= exp(-m.r * fog_density * thin * wisp * fog_extinction * step_len);
		if (keep < fog_done) break;
	}
	return 1.0 - keep;
}

