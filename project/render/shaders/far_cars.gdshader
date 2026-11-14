// The far traffic's cars (native/src/render/agents/far_traffic.h): each instance a car on a street, placed and moved here, never
// written a frame. The instance's transform is its street: the point (2u - u*u, 0, u*u) of its space is the street's centre at the
// share u of its length (the street is one curve: start, control point, end), so the car's place and heading come from
// MODEL_MATRIX alone. INSTANCE_CUSTOM = (its share of the street at clock 0, the shares it covers a second, the stretch of shares
// it moves round (0: it stands in the queue), its metres to the right of the centre); the instance's colour is its paint. The
// mesh is a car's hull, its front along +Z, its left along +X.
// A street of the window of detailed traffic has its pace written negative: its cars stand in for the detailed vehicles, which
// are drawn only within seen_reach of the point looked at and drawn_reach of the eye; there the stand-in is folded to nothing.
shader_type spatial;
render_mode skip_vertex_transform;

#include "res://render/shaders/color_space.gdshaderinc"

global uniform float night;
uniform float clock; // the simulation's seconds, wrapped (far_traffic.cpp)
uniform float hull_half; // half the hull's length, metres
uniform vec2 seen_at; // the ground point the view looks at (x, z), each frame
uniform float seen_reach; // metres round it within which the simulation publishes the detailed vehicles
uniform float drawn_reach; // metres round the eye, flat, within which the render draws them
uniform float body_roughness;
uniform vec3 head_glow : source_color;
uniform vec3 tail_glow : source_color;
uniform float head_power;
uniform float tail_power;
uniform float lamp_share; // of the half length from each end: what burns by night
varying float v_along; // the fragment's place along the hull, -1 its rear to 1 its front

void vertex() {
	float pace = abs(INSTANCE_CUSTOM.y);
	float share = INSTANCE_CUSTOM.z > 0.0 ? mod(INSTANCE_CUSTOM.x + pace * clock, INSTANCE_CUSTOM.z) : INSTANCE_CUSTOM.x;
	float u = clamp(share, 0.0, 1.0);
	vec3 at = (MODEL_MATRIX * vec4(2.0 * u - u * u, 0.0, u * u, 1.0)).xyz;
	vec2 run = (MODEL_MATRIX * vec4(2.0 - 2.0 * u, 0.0, 2.0 * u, 0.0)).xz;
	run = dot(run, run) > 1e-8 ? normalize(run) : vec2(1.0, 0.0);
	vec3 ahead = vec3(run.x, 0.0, run.y);
	vec3 left = vec3(run.y, 0.0, -run.x);
	v_along = VERTEX.z / max(hull_half, 0.01);
	// a car of the window's street where the detailed vehicle is drawn as itself: every vertex at one point, nothing drawn
	vec3 eye = INV_VIEW_MATRIX[3].xyz;
	bool stood_for = INSTANCE_CUSTOM.y < 0.0 && distance(at.xz, seen_at) <= seen_reach && distance(at.xz, eye.xz) <= drawn_reach;
	float shown = stood_for ? 0.0 : 1.0;
	vec3 world = at + (left * (VERTEX.x - INSTANCE_CUSTOM.w) + vec3(0.0, VERTEX.y, 0.0) + ahead * VERTEX.z) * shown;
	VERTEX = (VIEW_MATRIX * vec4(world, 1.0)).xyz;
	NORMAL = normalize(mat3(VIEW_MATRIX) * (left * NORMAL.x + vec3(0.0, NORMAL.y, 0.0) + ahead * NORMAL.z));
}

void fragment() {
	ALBEDO = linear_out(srgb_to_linear(COLOR.rgb));
	ROUGHNESS = body_roughness;
	METALLIC = 0.0;
	vec3 lamps = v_along > 1.0 - lamp_share ? source_linear(head_glow) * head_power : (v_along < lamp_share - 1.0 ? source_linear(tail_glow) * tail_power : vec3(0.0));
	EMISSION = linear_out(lamps * night);
}
