## The water plane: its own material (render/shaders/water.gdshader, parameters in palette.json), or one plain colour for the Optimisations
## tab to see what its shading costs.
class_name CityWater
extends MeshInstance3D

const WATER_SHADER := "res://render/shaders/water.gdshader"
const PLAIN_SHADER := "res://render/shaders/water_plain.gdshader"
const WATER_EXTENT_M := 70000.0 # past any map and its horizon: the sea's plane

var own: ShaderMaterial # the lakes and rivers of the core (render/water) wear it too


func _init(palette: Dictionary) -> void:
	var plane := PlaneMesh.new()
	plane.size = Vector2(WATER_EXTENT_M, WATER_EXTENT_M)
	own = ShaderMaterial.new()
	own.shader = load(WATER_SHADER)
	var kinds := WorldMaterials.uniform_kinds(own.shader)
	for key in palette.water:
		if key != "sea_level_m" and not key.begins_with("_"):
			own.set_shader_parameter(key, WorldMaterials.typed(kinds, key, palette.water[key]))
	var weather: Dictionary = DataTables.catalog_table("nature").weather
	for key in ["waves", "foam", "current"]: # the weather's defaults until a map's is read (CityWorld.set_water_look)
		own.set_shader_parameter(key, weather[key].default)
	mesh = plane
	material_override = own
	position.y = palette.water.sea_level_m
	cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	layers = int(CityStore.constants.render_layers.water)


## The plain colour goes on the material itself: the core's water changes with the sea.
func set_plain(plain: bool) -> void:
	own.shader = load(PLAIN_SHADER if plain else WATER_SHADER)
