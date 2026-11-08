// What marks the map itself on the ground (terrain.gdshader): the wall where the map ends, rock on its own plane and a shade darker,
// and the zoning grid the Layers popover lays over it, one line a cell, as wide as a pixel of it. Needs terrain_uniforms.gdshaderinc
// (map_half, grid_m, grid_alpha).
#ifndef MAP_FRAME_INC
#define MAP_FRAME_INC

global uniform float map_grid;

const float MAP_EDGE_INSET_M = 0.5; // the wall stands this far inside the map's half-side
const float MAP_EDGE_SHADE = 0.4; // what the wall loses of its light

bool map_edge(vec3 p) {
	return max(abs(p.x), abs(p.z)) >= map_half - MAP_EDGE_INSET_M;
}

// The ground's normal, or the wall's on the map's edge.
vec3 map_edge_normal(vec3 n, vec3 p) {
	if (!map_edge(p)) return n;
	vec2 past = abs(p.xz);
	return past.x > past.y ? vec3(sign(p.x), 0.0, 0.0) : vec3(0.0, 0.0, sign(p.z));
}

// The ground's colour with the wall's shade and, when the popover asks, the zoning grid over it.
vec3 map_frame_colour(vec3 c, vec3 p) {
	c *= map_edge(p) ? 1.0 - MAP_EDGE_SHADE : 1.0;
	if (map_grid > 0.5) {
		vec2 cell = abs(fract(p.xz / grid_m - 0.5) - 0.5) / fwidth(p.xz / grid_m);
		c = mix(c, vec3(1.0), (1.0 - clamp(min(cell.x, cell.y), 0.0, 1.0)) * grid_alpha);
	}
	return c;
}

#endif
