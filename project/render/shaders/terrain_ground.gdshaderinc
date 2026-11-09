// The ground's fragment, the body of fragment() in the terrain (terrain.gdshader) and in the ground seams beside the roads
// (terrain_seam.gdshader), so the two are one surface: grass map in world space, sand at the shore, soil and rock on slopes steeper
// than loose debris rests, the painted ground and gullies of the control maps (terrain_common.gdshaderinc). Reads UV (region
// space), v_vertex, v_ground_n and v_camera_pos.
	vec2 uv = UV; // Terrain3D injects editor overlays that read `uv`
	vec2 index_id = floor(uv);
	ivec3 i0 = get_index_coord(index_id + vec2(0, 1));
	ivec3 i2 = get_index_coord(index_id + vec2(1, 0));
	ivec3 i3 = get_index_coord(index_id);
	vec3 w_normal = normalize(v_ground_n);
	vec2 past = abs(v_vertex.xz);
	if (max(past.x, past.y) >= map_half - 0.5) { // the wall at the end of the map: rock on its own plane
		w_normal = past.x > past.y ? vec3(sign(v_vertex.x), 0.0, 0.0) : vec3(0.0, 0.0, sign(v_vertex.z));
	}
	ivec3 i1 = get_index_coord(index_id + vec2(1, 1));
	vec2 fc = fract(uv);
	PaintCorner p00 = read_paint(i3), p10 = read_paint(i2), p01 = read_paint(i0), p11 = read_paint(i1);
	vec4 cw = vec4((1.0 - fc.x) * (1.0 - fc.y), fc.x * (1.0 - fc.y), (1.0 - fc.x) * fc.y, fc.x * fc.y);
	float slope = 1.0 - w_normal.y;
	float gully = gully_weight(dot(cw, vec4(p00.gully, p10.gully, p01.gully, p11.gully)), dot(cw, vec4(p00.wipe, p10.wipe, p01.wipe, p11.wipe)), slope);
	float gully_shade = 1.0;
	if (gully > 0.0) w_normal = gully_normal(w_normal, v_vertex.xz, gully, gully_shade);
	float detail = 1.0 - smoothstep(detail_fade_m.x, detail_fade_m.y, length(v_vertex.xz - v_camera_pos.xz));
	vec3 tint = biome_tint[map_biome];
	float biome_cover = dot(cw, vec4(p00.biome_amount, p10.biome_amount, p01.biome_amount, p11.biome_amount));
	if (biome_cover > 0.0) tint = mix(tint, biome_tint[(cw.x + cw.y > cw.z + cw.w ? (cw.x > cw.y ? p00.biome : p10.biome) : (cw.z > cw.w ? p01.biome : p11.biome))], biome_cover);
	float per_pixel = max(fwidth(v_vertex.x), fwidth(v_vertex.z));
	vec3 eco = ecology_at(v_vertex.xz);
	// The rock the land is made of: the painted geology (sandstone, basalt), granite where none was.
	vec4 sand_at = vec4(float(p00.surface == PAINT_SANDSTONE) * p00.amount, float(p10.surface == PAINT_SANDSTONE) * p10.amount,
			float(p01.surface == PAINT_SANDSTONE) * p01.amount, float(p11.surface == PAINT_SANDSTONE) * p11.amount);
	vec4 basalt_at = vec4(float(p00.surface == PAINT_BASALT) * p00.amount, float(p10.surface == PAINT_BASALT) * p10.amount,
			float(p01.surface == PAINT_BASALT) * p01.amount, float(p11.surface == PAINT_BASALT) * p11.amount);
	vec2 rocks = vec2(dot(cw, sand_at), dot(cw, basalt_at));
	vec3 geology = vec3(max(0.0, 1.0 - rocks.x - rocks.y), rocks);
	vec3 g;
	float n;
	vec3 c = grass_ground(v_vertex.xz, detail, tint, eco.x, per_pixel, g, n);
	vec3 wdx = dFdx(v_vertex), wdy = dFdy(v_vertex);
	vec3 soil_k = soil_tint(geology);
	c = ecology_over(c, g, eco, land(dirt_layer, fract(v_vertex.xz / dirt_m), wdx.xz / dirt_m, wdy.xz / dirt_m) * soil_k);
	float wander = (n - 0.5) * slope_wander;
	float dirt_w = smoothstep(dirt_from, dirt_to, slope + wander) * (1.0 - smoothstep(rock_slope, rock_slope + rock_edge, slope));
	float rock_w = smoothstep(rock_slope, rock_slope + rock_edge, slope + wander);
	vec2 tilt = grass_tilt(v_vertex.xz, per_pixel, 1.0 - max(dirt_w, rock_w));
	NORMAL = mat3(VIEW_MATRIX) * normalize(w_normal + vec3(tilt.x, 0.0, tilt.y));
	if (dirt_w > 0.0) {
		c = mix(c, land_triplanar(dirt_layer, v_vertex / dirt_m, wdx / dirt_m, wdy / dirt_m, w_normal) * soil_k, dirt_w);
	}
	c = shore_over(c, v_vertex, slope, wdx, wdy);
	if (rock_w > 0.0) {
		c = mix(c, mix(rock_strata(v_vertex, wdx, wdy, w_normal, geology), rock, rock_flat_share), rock_w);
	}
	c = paint_over(c, g, p00, p10, p01, p11, cw, v_vertex, wdx, wdy, w_normal);
	c *= gully_shade;
	c *= 1.0 - wall_dim * step(map_half - 0.5, max(abs(v_vertex.x), abs(v_vertex.z)));
	if (map_grid > 0.5) { // the Layers popover: the zoning grid over the ground, one line a cell, as wide as a pixel of it
		vec2 cell = abs(fract(v_vertex.xz / grid_m - 0.5) - 0.5) / fwidth(v_vertex.xz / grid_m);
		c = mix(c, vec3(1.0), (1.0 - clamp(min(cell.x, cell.y), 0.0, 1.0)) * grid_alpha);
	}
	ALBEDO = c;
	ROUGHNESS = mix(ground_rough.x, ground_rough.y, rain_wet);
