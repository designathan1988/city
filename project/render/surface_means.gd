## The mean colour of each surface map and the road's tones, computed once and sent as uniforms (road.gdshader divides by
## them): the shader no longer samples the smallest mip every pixel.
class_name SurfaceMeans
extends RefCounted

const MEAN_PX := 64 # side of the shrunk copy whose average becomes a surface map's mean colour
const LANE_TONES := 8 # slots of road_look.h TONE_SLOTS
const MEAN_MAPS := ["asphalt_map", "footway_map", "kerb_map"] # those road.gdshader tones by their mean; the grass is its map


## `load_param` turns a value of the material table into what the shader takes (CityWorld.shader_value).
static func apply(m: ShaderMaterial, params: Dictionary, load_param: Callable) -> void:
	for name in MEAN_MAPS:
		m.set_shader_parameter("mean_" + name.trim_suffix("_map"), mean_color(load_param.call(params[name])))
	var streets: Dictionary = DataTables.render_table("streets")
	m.set_shader_parameter("asphalt_reference", Color.html(streets.surface_colors[0]))
	var tones := PackedVector3Array()
	for road_class in streets.asphalt_by_class:
		var c := Color.html(streets.asphalt_by_class[road_class]).srgb_to_linear()
		tones.push_back(Vector3(c.r, c.g, c.b))
	tones.resize(LANE_TONES)
	m.set_shader_parameter("lane_tone", tones)
	m.set_shader_parameter("plate_blend", streets.plate_blend_m)
	var walk := Color.html(streets.paving_colors[0]).srgb_to_linear()
	m.set_shader_parameter("footway_tone", Vector3(walk.r, walk.g, walk.b))


static func mean_color(tex: Texture2D) -> Color:
	if tex == null:
		return Color(1, 1, 1)
	var img := tex.get_image()
	if img == null:
		return Color(1, 1, 1)
	img = img.duplicate()
	if img.is_compressed():
		if img.decompress() != OK:
			return Color(1, 1, 1)
	# Trilinear reads the mipmaps, so every pixel of the map counts: bilinear only looks at a few, and on a map of pavers
	# those missed the dark joints. The mean of light is taken in linear, and the result goes back as a colour.
	img.resize(MEAN_PX, MEAN_PX, Image.INTERPOLATE_TRILINEAR)
	var sum := Vector3.ZERO
	for y in MEAN_PX:
		for x in MEAN_PX:
			var c := img.get_pixel(x, y).srgb_to_linear()
			sum += Vector3(c.r, c.g, c.b)
	var n := float(MEAN_PX * MEAN_PX)
	return Color(sum.x / n, sum.y / n, sum.z / n).linear_to_srgb()
