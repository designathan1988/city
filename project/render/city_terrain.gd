## The Terrain3D node as the game wants it: the ground's own shader, no collision (picking is the core's raycast), no shadow of its own;
## and the ground seams beside the roads drawn in the same surface (feed_seams).
class_name CityTerrain
extends RefCounted

const PLACEHOLDER_TEX_PX := 16 # a stand-in texture's side, until the real one loads


static func make(mat: Resource, shader: Shader) -> Node3D:
	var terrain: Node3D = ClassDB.instantiate(&"Terrain3D")
	terrain.name = "Terrain3D"
	mat.shader_override = shader
	mat.shader_override_enabled = true
	terrain.material = mat
	# One tiny placeholder texture keeps off Terrain3D's checkered debug view: the override shader paints its own
	var assets: Resource = ClassDB.instantiate(&"Terrain3DAssets")
	var tex_asset: Resource = ClassDB.instantiate(&"Terrain3DTextureAsset")
	var img := Image.create(PLACEHOLDER_TEX_PX, PLACEHOLDER_TEX_PX, true, Image.FORMAT_RGBA8)
	img.fill(Color.WHITE)
	img.generate_mipmaps()
	tex_asset.albedo_texture = ImageTexture.create_from_image(img)
	assets.set_texture_asset(0, tex_asset)
	terrain.assets = assets
	terrain.collision_mode = 0 # disabled: picking uses the core's raycast
	terrain.free_editor_textures = false
	terrain.cast_shadows = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF # the ground only receives
	terrain.render_layers = int(CityStore.constants.render_layers.terrain)
	return terrain


## The ground seams beside the roads are the terrain's own surface (render/shaders/terrain_seam.gdshader), fed through Terrain3D's
## public API each frame: its maps as texture arrays (Terrain3DData get_height_maps_rid, get_control_maps_rid, get_region_map), its
## vertex spacing and region size, the clipmap's target (the fragment's distance blends read it), and the look's uniforms
## (Terrain3DMaterial.get_shader_param). `looks` keeps the names of those uniforms, listed on the first call.
static func feed_seams(terrain: Node3D, mat: Resource, seam: ShaderMaterial, target: Node3D, looks: Array[StringName]) -> void:
	if looks.is_empty():
		for u: Dictionary in seam.shader.get_shader_uniform_list():
			if not String(u.name).begins_with("_"):
				looks.append(StringName(u.name))
	for name in looks:
		seam.set_shader_parameter(name, mat.get_shader_param(name))
	var to := seam.get_rid()
	var data: Object = terrain.get("data")
	var spacing: float = terrain.get("vertex_spacing")
	var region: float = float(terrain.get("region_size"))
	RenderingServer.material_set_param(to, &"_height_maps", data.call("get_height_maps_rid"))
	RenderingServer.material_set_param(to, &"_control_maps", data.call("get_control_maps_rid"))
	RenderingServer.material_set_param(to, &"_region_map", data.call("get_region_map"))
	RenderingServer.material_set_param(to, &"_vertex_spacing", spacing)
	RenderingServer.material_set_param(to, &"_vertex_density", 1.0 / spacing)
	RenderingServer.material_set_param(to, &"_region_size", region)
	RenderingServer.material_set_param(to, &"_region_texel_size", 1.0 / region)
	if target:
		RenderingServer.material_set_param(to, &"_target_pos", target.global_position)
