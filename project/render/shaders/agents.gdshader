// Vehicles (native/src/render/agents). UV2.x tags each part (AgentTag in agent_meshes.h): 0 fixed colour, 1 paint,
// 2 headlamp, 3 tail lamp (brake), 4 glass (opaque: the far level; the near one's is agent_glass.gdshader),
// 5 a lit sign (the bus's destination, day and night), 6 a roof (the paint, or black on a share of the cars), 7 the gloss black round
// the glass, 8 and 9 the left and the right indicator (lit on the side INSTANCE_CUSTOM.a names: 1 left, 2 right; lit, its lens keeps a least size on the screen: lens_grow). The lenses of a vehicle standing with nobody aboard are off, their vertex colours; on anything else they burn, day and
// night. UV2.y = 1 marks wheel vertices: they turn about X round their hub (UV = hub y, z). INSTANCE_CUSTOM = (paint index, wheel roll
// angle + 8 when braking, passengers, 0); COLOR.a is the vehicle's seed. The Compatibility renderer stores instance custom data as half
// floats: whole numbers below 2048 exactly. Who rides is drawn by PeopleRenderer.
shader_type spatial;

#include "res://render/shaders/color_space.gdshaderinc"

global uniform float night;
uniform vec3 head_glow : source_color;
uniform vec3 tail_glow : source_color;
uniform float head_power;
uniform float tail_power;
uniform float brake_ratio; // a stop lamp's light against its rear position lamp's
uniform vec3 head_lit; // a lens burning, by day: the radiance the tone mapping shows as its colour (render/tone_map.gd)
uniform vec3 tail_lit;
uniform vec3 brake_lit;
uniform vec3 signal_lit; // an indicator flashing, by day
uniform float signal_hz; // flashes a second
uniform float signal_duty; // the share of a flash it is lit
uniform float signal_least_px; // a lit indicator's lens covers at least this many pixels
uniform float signal_most_m; // and is never drawn larger than this, metres
uniform vec3 dark_roof : source_color;
uniform float dark_roof_share;
uniform vec3 sign_glow : source_color;
uniform float sign_power;
// roughness by surface (materials.json agent: sources there); every surface is a dielectric
uniform float paint_roughness;
uniform vec3 sheen_sky : source_color; // what a clear coat mirrors above the horizon and below it
uniform vec3 sheen_ground : source_color;
uniform float sheen_power;
uniform float lens_roughness;
uniform float trim_roughness;

const float BRAKE_FLAG = 8.0;
const float NEAREST_DEPTH_M = 0.05; // nothing is measured nearer the eye than this
const float LENS_MARK = 3.5; // UV2.y over this: an indicator's lens, UV its centre (x, y) on its end (agent_meshes.h Limb::LENS)
#include "res://render/shaders/agent_door.gdshaderinc"
// the paints as numbers (vec3, not colours): used as they are, like the parts' own vertex colours; the riders' helmets too
uniform vec3 paints[24]; // the paints, then the colours of the transport lines
varying flat float v_tag;
varying flat vec4 v_custom;
varying flat float v_seed;
// A number in [0, 1) from the vehicle's seed, a seat and a purpose.
float pick(float seed, float seat, float what) {
	return fract(sin(dot(vec3(seed * 251.0, seat, what), vec3(12.9898, 78.233, 37.719))) * 43758.5453);
}

// An indicator's lens that is lit now keeps a least size on the screen, as a light point does: a lamp is seen by its light, not by
// its lens's size. Its vertices carry its centre on its end (`centre`, x and y); it grows round it until its larger side covers
// signal_least_px pixels, never past signal_most_m; grown, it faces the eye (its corners laid on the view's own axes: from above
// the end it stands on is edge-on) and comes toward the eye by its grown half size, clear of the body's curve.
void lens_grow(inout vec3 p, vec2 centre, mat4 model_view, mat4 projection, vec2 viewport) {
	float side = max(abs(p.x - centre.x), abs(p.y - centre.y)) * 2.0;
	vec3 seen = (model_view * vec4(centre, p.z, 1.0)).xyz; // its centre in the view's space
	bool parallel = projection[3][3] != 0.0;
	// a metre there on the screen, in pixels: the projection's scale (its magnitude: the renderer may flip Y) over the view's depth,
	// or as it is in a parallel view
	float depth = parallel ? 1.0 : max(-seen.z, NEAREST_DEPTH_M);
	float px_per_m = viewport.y * 0.5 * abs(projection[1][1]) / depth;
	float grow = min(signal_least_px / max(side * px_per_m, 0.001), signal_most_m / max(side, 0.001));
	if (grow <= 1.0) {
		return;
	}
	vec3 to_eye = parallel ? vec3(0.0, 0.0, 1.0) : normalize(-seen);
	vec3 laid = vec3((p.xy - centre) * grow, 0.0) + to_eye * (side * 0.5 * grow); // in the view's space, from its centre
	p = vec3(centre, p.z) + laid * mat3(model_view); // back to the model's (the instance is turned and moved, never scaled)
}

void vertex() {
	v_tag = UV2.x;
	v_custom = INSTANCE_CUSTOM;
	v_seed = COLOR.a;
	if (UV2.y > LENS_MARK) {
		// never in the shadow pass: the sun's projection is no screen
		if (!IN_SHADOW_PASS && INSTANCE_CUSTOM.b > -0.5 && int(INSTANCE_CUSTOM.a + 0.5) == int(UV2.x + 0.5) - 7 && fract(TIME * signal_hz + COLOR.a) < signal_duty) {
			lens_grow(VERTEX, UV, MODELVIEW_MATRIX, PROJECTION_MATRIX, VIEWPORT_SIZE);
		}
	} else if (UV2.y > DOOR_MARK) {
		door_swing(VERTEX, NORMAL, UV, INSTANCE_CUSTOM.g, UV2.y);
	} else if (UV2.y > 0.5) {
		float a = mod(INSTANCE_CUSTOM.g, BRAKE_FLAG);
		vec2 d = VERTEX.yz - UV;
		vec2 nd = NORMAL.yz;
		float c = cos(a), s = sin(a);
		VERTEX.yz = UV + vec2(d.x * c - d.y * s, d.x * s + d.y * c);
		NORMAL.yz = vec2(nd.x * c - nd.y * s, nd.x * s + nd.y * c);
	}
}

// The clear coat of a painted panel: the world mirrored by Schlick's Fresnel, the sky over a soft horizon and the street under it. That line
// running along the doors and the sky growing on the roof at a glancing view is what reads as car paint.
vec3 sheen(vec3 n, vec3 v) {
	vec3 r = reflect(-v, n);
	float f = 0.04 + 0.96 * pow(1.0 - clamp(dot(n, v), 0.0, 1.0), 5.0);
	vec3 world = mix(source_linear(sheen_ground), source_linear(sheen_sky) * (0.7 + 0.5 * clamp(r.y, 0.0, 1.0)), smoothstep(-0.04, 0.06, r.y));
	return world * f * sheen_power * (1.0 - night);
}

void fragment() {
	int tag = int(v_tag + 0.5);
	vec3 c = srgb_to_linear(COLOR.rgb); // COLOR is sRGB on both renderers; everything below in linear
	float rough = trim_roughness;
	vec3 emit = vec3(0.0);
	vec3 coat = sheen(mat3(INV_VIEW_MATRIX) * NORMAL, mat3(INV_VIEW_MATRIX) * VIEW);
	if (tag == 1) {
		c = srgb_to_linear(paints[int(v_custom.r + 0.5)]);
		rough = paint_roughness;
		emit = coat;
	} else if (tag == 6) {
		c = pick(v_seed, 0.0, 9.0) < dark_roof_share ? source_linear(dark_roof) : srgb_to_linear(paints[int(v_custom.r + 0.5)]);
		rough = paint_roughness;
		emit = coat;
	} else if (tag == 7) {
		rough = paint_roughness;
		emit = coat;
	} else if (tag == 2) {
		bool on = v_custom.b > -0.5;
		emit = on ? head_lit + source_linear(head_glow) * night * head_power : vec3(0.0);
		c = on ? vec3(0.0) : c;
		rough = lens_roughness;
	} else if (tag == 3) {
		bool on = v_custom.b > -0.5;
		bool braking = mod(v_custom.g, DOOR_FLAG) >= BRAKE_FLAG; // by day each shows as its colour: more red than that, and ACES turns it cream
		emit = on ? (braking ? brake_lit : tail_lit) + source_linear(tail_glow) * tail_power * night * (braking ? brake_ratio : 1.0) : vec3(0.0);
		c = on ? vec3(0.0) : c;
		rough = lens_roughness;
	} else if (tag == 8 || tag == 9) {
		// the indicator of the side the vehicle signals, flashing, each vehicle on its own beat
		bool on = v_custom.b > -0.5 && int(v_custom.a + 0.5) == tag - 7 && fract(TIME * signal_hz + v_seed) < signal_duty;
		emit = on ? signal_lit + source_linear(sign_glow) * tail_power * night * brake_ratio : vec3(0.0);
		c = on ? vec3(0.0) : c;
		rough = lens_roughness;
	} else if (tag == 4) {
		rough = lens_roughness;
	} else if (tag == 5) {
		emit = source_linear(sign_glow) * sign_power;
		rough = lens_roughness;
	}
	ALBEDO = linear_out(c);
	ROUGHNESS = rough;
	METALLIC = 0.0;
	EMISSION = linear_out(emit);
}
