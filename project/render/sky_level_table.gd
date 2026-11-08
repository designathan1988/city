## The clear sky's light on level ground over the sun's heights, and the town's light in a sky with no sun (render/shaders/
## sky_level_table.gdshader), rendered once on the first frame and read back once: render/daylight.gd meters the eye's adaptation by it
## (render/eye.gd). The shader is the one owner of the sky's light; this keeps the row it gave, nothing on the CPU integrates the sky.
class_name SkyLevelTable
extends SubViewport

signal filled

const SHADER := "res://render/shaders/sky_level_table.gdshader"
const STEPS := 64 # sun heights, evenly in sin(elevation), read between by a straight line: the meter needs no finer step
## The lowest sun of the row: 6 degrees under, the end of civil twilight, past where the sky's own light ends in this air (its earth
## shadow at the Rayleigh scale height, sky_model.gdshaderinc sky_sun_at).
const SUN_LOWEST_SINE := -0.1045
const TOWN_TEXELS := 2 # the town's level light and zenith

var levels := PackedVector3Array() # by the sun's height, a white floor's radiance in the lights' unit
var town_level := Vector3.ZERO # the town's light at radiance 1: its level light
var town_zenith := Vector3.ZERO # and its zenith


func _init() -> void:
	size = Vector2i(STEPS + TOWN_TEXELS, 1)
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
	if img == null or img.get_width() < STEPS + TOWN_TEXELS:
		push_error("SkyLevelTable: the sky's row was not rendered")
		return
	levels.resize(STEPS)
	for x in STEPS:
		levels[x] = _texel(img, x)
	town_level = _texel(img, STEPS)
	town_zenith = _texel(img, STEPS + 1)
	filled.emit()


func is_filled() -> bool:
	return not levels.is_empty()


## The clear sky's level light with the sun at sin(elevation) `mu`, between the row's texels (none under its lowest sun).
func level(mu: float) -> Vector3:
	var at := (mu - SUN_LOWEST_SINE) / (1.0 - SUN_LOWEST_SINE) * float(STEPS - 1)
	if at <= 0.0:
		return Vector3.ZERO if mu < SUN_LOWEST_SINE else levels[0]
	var i := mini(int(at), STEPS - 2)
	return levels[i].lerp(levels[i + 1], minf(at - float(i), 1.0))


static func _texel(img: Image, x: int) -> Vector3:
	var c := img.get_pixel(x, 0)
	return Vector3(c.r, c.g, c.b)
