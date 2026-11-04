## The water plane: its own material (render/shaders/water.gdshader, parameters in palette.json), or one plain colour for the Optimisations
## tab to see what its shading costs.
class_name CityWater
extends MeshInstance3D

const WATER_SHADER := "res://render/shaders/water.gdshader"
const WATER_EXTENT_M := 70000.0

var _own: ShaderMaterial
var _flat: StandardMaterial3D


func _init(palette: Dictionary) -> void:
	var plane := PlaneMesh.new()
	plane.size = Vector2(WATER_EXTENT_M, WATER_EXTENT_M)
	_own = ShaderMaterial.new()
	_own.shader = load(WATER_SHADER)
	for key in palette.water:
		if key != "sea_level_m" and not key.begins_with("_"):
			_own.set_shader_parameter(key, CityWorld.shader_value(palette.water[key]))
	# The ripples move the way the wind blows: the vegetation material holds the wind (its single owner).
	var wind: Dictionary = DataTables.render().materials.vegetation.params
	_own.set_shader_parameter("wind_x", wind.wind_dir_x)
	_own.set_shader_parameter("wind_z", wind.wind_dir_z)
	_flat = StandardMaterial3D.new()
	_flat.albedo_color = Color.html(palette.water.deep)
	mesh = plane
	material_override = _own
	position.y = palette.water.sea_level_m
	cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	layers = int(CityStore.constants.render_layers.water)


func set_plain(plain: bool) -> void:
	material_override = _flat if plain else _own
