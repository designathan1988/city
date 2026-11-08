## The clear sky's light on level ground over the sun's heights, and the town's light in a sky with no sun (render/shaders/
## sky_level_table.gdshader), and the same at the horizon (the haze's colour), rendered once on the first frame and read back once:
## render/light_meter.gd meters the eye's adaptation by it (render/eye.gd) and colours the haze where the engine cannot. The shader is the one owner of the sky's light; this keeps the row it gave, nothing on the CPU integrates the sky.
class_name SkyLevelTable
extends SubViewport

signal filled

const SHADER := "res://render/shaders/sky_level_table.gdshader"
const STEPS := 64 # sun heights, evenly in sin(elevation), read between by a straight line: the meter needs no finer step
## The lowest sun of the row: 18 degrees under, the end of astronomical twilight, where the sky's last sunlit layer is gone
## (sky_model.gdshaderinc sky_layer_lit: the earth's shadow 320 km up).
const SUN_LOWEST_SINE := -0.309
const TOWN_TEXELS := 2 # the town's level light and zenith
const TABLE_ROWS := 2 # the level light, the horizon

var levels := PackedVector3Array() # by the sun's height, a white floor's radiance in the lights' unit
var town_level := Vector3.ZERO # the town's light at radiance 1: its level light
var town_zenith := Vector3.ZERO # and its zenith
var horizons := PackedVector3Array() # the clear sky at the horizon, by the sun's height
var town_horizon := Vector3.ZERO # the town's light at radiance 1 at the horizon


func _init() -> void:
	size = Vector2i(STEPS + TOWN_TEXELS, TABLE_ROWS)
	use_hdr_2d = true # the light in linear floats
	transparent_bg = false
	disable_3d = true
	render_target_update_mode = SubViewport.UPDATE_ONCE
	var material := ShaderMaterial.new()
	material.shader = load(SHADER)
	material.set_shader_parameter("steps", STEPS)
	material.set_shader_parameter("mu_low", SUN_LOWEST_SINE)
	var rect := ColorRect.new()
	rect.size = Vector2(size)
	rect.material = material
	add_child(rect)


func _ready() -> void:
	if DisplayServer.get_name() == "headless": # nothing is rendered: no light to meter, the exposure stays the day's
		return
	await RenderingServer.frame_post_draw
	var img := get_texture().get_image()
	if img == null or img.get_width() < STEPS + TOWN_TEXELS or img.get_height() < TABLE_ROWS:
		push_error("SkyLevelTable: the sky's rows were not rendered")
		return
	levels.resize(STEPS)
	horizons.resize(STEPS)
	for x in STEPS:
		levels[x] = _texel(img, x, 0)
		horizons[x] = _texel(img, x, 1)
	town_level = _texel(img, STEPS, 0)
	town_zenith = _texel(img, STEPS + 1, 0)
	town_horizon = _texel(img, STEPS, 1)
	filled.emit()


func is_filled() -> bool:
	return not levels.is_empty()


## The clear sky's level light with the sun at sin(elevation) `mu`, between the row's texels (none under its lowest sun).
func level(mu: float) -> Vector3:
	return _between(levels, mu)


## The clear sky at the horizon with the sun at sin(elevation) `mu`.
func horizon(mu: float) -> Vector3:
	return _between(horizons, mu)


static func _between(row: PackedVector3Array, mu: float) -> Vector3:
	var at := (mu - SUN_LOWEST_SINE) / (1.0 - SUN_LOWEST_SINE) * float(STEPS - 1)
	if at <= 0.0:
		return Vector3.ZERO if mu < SUN_LOWEST_SINE else row[0]
	var i := mini(int(at), STEPS - 2)
	return row[i].lerp(row[i + 1], minf(at - float(i), 1.0))


static func _texel(img: Image, x: int, y: int) -> Vector3:
	var c := img.get_pixel(x, y)
	return Vector3(c.r, c.g, c.b)
