// What a pass reads of the depth: a pixel's place in view space and the normal of the surface there. One owner for the passes that
// read it (occlusion, contact, atmosphere) and the decals laid on it (the lamps' pools, the ground marks).

uniform sampler2D depth_texture : hint_depth_texture, repeat_disable, filter_nearest;

// A perspective's inverse has six terms that count, not sixteen: this runs tens of times a pixel, so only they are worked.
vec3 view_at(vec2 uv, mat4 inv_projection) {
	float depth = textureLod(depth_texture, uv, 0.0).x;
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	depth = depth * 2.0 - 1.0;
#endif
	vec2 xy = (uv * 2.0 - 1.0) * vec2(inv_projection[0][0], inv_projection[1][1]) + inv_projection[3].xy;
	return vec3(xy, inv_projection[2][2] * depth + inv_projection[3][2]) / (inv_projection[2][3] * depth + inv_projection[3][3]);
}

// The surface's normal from the depth, each axis from the side that lies on the same surface as the pixel.
vec3 view_normal(vec3 p, vec2 uv, vec2 texel, mat4 inv_projection) {
	vec3 l = view_at(uv - vec2(texel.x, 0.0), inv_projection), r = view_at(uv + vec2(texel.x, 0.0), inv_projection);
	vec3 b = view_at(uv - vec2(0.0, texel.y), inv_projection), t = view_at(uv + vec2(0.0, texel.y), inv_projection);
	vec3 dx = abs(l.z - p.z) < abs(r.z - p.z) ? p - l : r - p;
	vec3 dy = abs(b.z - p.z) < abs(t.z - p.z) ? p - b : t - p;
	vec3 n = normalize(cross(dx, dy));
	return dot(n, p) > 0.0 ? -n : n;
}
