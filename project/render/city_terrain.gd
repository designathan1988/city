## The Terrain3D node as the game wants it: the ground's own shader, no collision (picking is the core's raycast), no shadow of its own.
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
