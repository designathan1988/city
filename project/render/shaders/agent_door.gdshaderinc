// The kerb door of a vehicle swung by the vertex stage, the way a wheel turns: its vertices are marked in the mesh (UV2.y, UV the hinge)
// and the instance says how far it stands open, over the wheel's angle (agent_renderer.cpp DOOR_FLAG), so a door costs no draw.
const float DOOR_FLAG = 16.0;
const float DOOR_LEVELS = 31.0; // pose_buffer.h LOOK_DOOR_MASK
const float DOOR_SWING = 1.15; // radians, fully open (Roadcraft occupants.ts)
const float DOOR_MARK = 1.5; // UV2.y over this: a door's vertex
const float DOOR_BACK_MARK = 2.5; // and over this, one hung at its back edge

const float SILL_SPAN_M = 4.0; // a pane's COLOR.r: its sill's height over this
const float PANE_MARK = 0.5; // COLOR.g over this: a front door's pane, which goes down

// How far a car's front window on the side of `x` is down: three cars in ten with one or both open, half of those part-way (Roadcraft
// agents.ts windowsDown, windowDrop). `seed` the vehicle's own.
float pane_drop(float seed, float x) {
	float pick = fract(seed * 91.7), part = fract(seed * 37.3);
	bool down = pick >= 0.9 || (pick >= 0.8 && x > 0.0) || (pick >= 0.7 && pick < 0.8 && x < 0.0);
	return down ? (part < 0.5 ? 0.45 : 1.0) : 0.0;
}

void door_swing(inout vec3 p, inout vec3 n, vec2 hinge, float wheel_and_door, float mark) {
	float a = floor(wheel_and_door / DOOR_FLAG) / DOOR_LEVELS * DOOR_SWING * sign(hinge.x) * (mark > DOOR_BACK_MARK ? -1.0 : 1.0); // always out of the body
	float c = cos(a), s = sin(a);
	vec2 d = p.xz - hinge;
	p.xz = hinge + vec2(d.x * c - d.y * s, d.x * s + d.y * c);
	n.xz = vec2(n.x * c - n.z * s, n.x * s + n.z * c);
}
