## The water plane: its own material (render/shaders/water.gdshader, parameters in palette.json), or one plain colour for the Optimisations
## tab to see what its shading costs.
class_name CityWater
extends MeshInstance3D

const WATER_SHADER := "res://render/shaders/water.gdshader"
const PLAIN_SHADER := "res://render/shaders/water_plain.gdshader"
const WATER_EXTENT_M := 70000.0

var own: ShaderMaterial # the lakes and rivers of the core (render/water) wear it too


func _init(palette: Dictionary) -> void:
	var plane := PlaneMesh.new()
	plane.size = Vector2(WATER_EXTENT_M, WATER_EXTENT_M)
	own = ShaderMaterial.new()
	own.shader = load(WATER_SHADER)
	for key in palette.water:
		if key != "sea_level_m" and not key.begins_with("_"):
			own.set_shader_parameter(key, CityWorld.shader_value(palette.water[key]))
	mesh = plane
	material_override = own
	position.y = palette.water.sea_level_m
	cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	layers = int(CityStore.constants.render_layers.water)


## The plain colour goes on the material itself: the core's water changes with the sea.
func set_plain(plain: bool) -> void:
	own.shader = load(PLAIN_SHADER if plain else WATER_SHADER)
