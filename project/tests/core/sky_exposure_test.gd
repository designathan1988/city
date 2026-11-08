## The sky's light pre-exposed once, on each renderer (tools/tasks runs it on Forward+ and on Compatibility): the sun off and the
## sky the only light, a white face shows a light that follows the eye's pre-exposure (render/light_meter.gd hold) once: held at a
## quarter, the light read back through the tone mapping's inverse is a quarter of it (applied twice it would be a sixteenth, not at
## all the same), so the sky's radiance map, its ambient and its reflections are each pre-exposed once (render/light_meter.gd expose
## says how the engine takes them). Under the day's sky, whose light reads in the middle of the screen's range with no exposure
## forced past what the map holds (Compatibility's map stops at 1). The tolerance is two 8-bit steps of the darker reading.
## Run: godot --path project [--rendering-method gl_compatibility] res://tests/core/sky_exposure_test.tscn -- --no-flow
extends TestCase

const VIEW_M := 9.0 # orthographic camera's height
const CAMERA_Z := 5.0
const FRAMES := 10 # the tone mapping settles
const BYTE := 255.0
const SKY_AT := Vector3(0.0, 2.0, 0.0) # inside the vignette's untouched middle
const SKY_STEP := 4.0 # the second reading's pre-exposure, a quarter of the first's
const COMPATIBILITY := "gl_compatibility"
const GLES_OFFSET := 0.055 # the sRGB power segment, 1.055 x^(1 / 2.4) - 0.055, as Compatibility encodes its output
const GLES_POWER := 2.4
const SKY_DECISIVE := 0.375 # half the way from a quarter's ratio to never's (1 / SKY_STEP against 1): a tolerance past it tells nothing

var cam: Camera3D


func _ready() -> void:
	world = CityWorld.new()
	add_child(world)
	cam = Camera3D.new()
	cam.projection = Camera3D.PROJECTION_ORTHOGONAL
	cam.size = VIEW_M
	cam.position = Vector3(0, 0, CAMERA_Z)
	add_child(cam)
	cam.make_current()
	if not world.sky_table.is_filled(): # the eye's meter reads the sky's row: the pre-exposure is its own once it is in
		await world.sky_table.filled
	await _sky_once()
	print("renderer: ", RenderingServer.get_current_rendering_method())
	finish()


func _sky_once() -> void:
	world.set_hour(DataTables.game("daylight").day_hour)
	world.environment.ambient_light_sky_contribution = 1.0
	world.sun.light_energy = 0.0
	world.environment.glow_enabled = false
	var face := MeshInstance3D.new()
	face.mesh = QuadMesh.new()
	var white := StandardMaterial3D.new()
	white.roughness = 1.0
	face.material_override = white
	face.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	face.position = SKY_AT
	add_child(face)
	var meter: LightMeter = world.daylight.meter
	await _frames(_sky_frames())
	var bright: float = meter.pre_exposure # the meter's own: no light forced past what the map holds
	meter.hold(bright)
	await _frames(_sky_frames())
	var high := _scene_light(SKY_AT)
	check(high > 0.0, "the sky lights a white face: %f" % high)
	meter.hold(bright / SKY_STEP)
	await _frames(_sky_frames())
	var low := _scene_light(SKY_AT)
	var step_off := _scene_light_step(SKY_AT) # one 8-bit step at the darker reading, as a share of it
	check(step_off * 2.0 < SKY_DECISIVE, "the readings tell once from never and twice: a step is %.3f of the darker" % step_off)
	var ratio := high / maxf(low, 1e-9) # each reading off by up to a step: the ratio by up to two of the darker one's
	var share := 1.0 / ratio
	var what := "the sky's light at a quarter of the pre-exposure: %.3f of it (%.3f wanted, %.3f a step)" % [share, 1.0 / SKY_STEP, step_off]
	if RenderingServer.get_current_rendering_method() == COMPATIBILITY:
		# Its radiance map is mipmapped and filtered on its sRGB codes (drivers/gles3: the raw map's mipmaps, effects/cubemap_filter.glsl
		# between the cubic decoding and the power encoding), so its ambient bends from proportional in the dark: the share must lie
		# nearer a quarter than never (1) or twice (a sixteenth), between their geometric middles.
		check(share > 1.0 / (SKY_STEP * sqrt(SKY_STEP)) and share < 1.0 / sqrt(SKY_STEP), what)
	else:
		check(absf(ratio / SKY_STEP - 1.0) <= step_off * 2.0, what)
	face.queue_free()


## The frames a change of the sky takes to reach its ambient: the sky's radiance map is redrawn once the pass in hand is done and then
## filtered a roughness layer a frame (renderer_rd/environment/sky.cpp update_radiance_buffers, incremental), two passes at most.
func _sky_frames() -> int:
	return 2 * int(ProjectSettings.get_setting("rendering/reflections/sky_reflections/roughness_layers")) + FRAMES


## The scene's light after the pre-exposure at `at`, read back through the Environment's tone mapping (render/tone_map.gd), its luminance.
func _scene_light(at: Vector3) -> float:
	return _light_of(get_viewport().get_texture().get_image().get_pixelv(Vector2i(cam.unproject_position(at).round())))


## What one 8-bit step of the screen at `at` is of the scene's light there.
func _scene_light_step(at: Vector3) -> float:
	var got := get_viewport().get_texture().get_image().get_pixelv(Vector2i(cam.unproject_position(at).round()))
	var up := Color(got.r + 1.0 / BYTE, got.g + 1.0 / BYTE, got.b + 1.0 / BYTE)
	return _light_of(up) / maxf(_light_of(got), 1e-9) - 1.0


## A screen colour's scene light: decoded by the output transfer of the renderer that drew it (Compatibility encodes with the sRGB
## power segment alone, drivers/gles3/shaders/tonemap_inc.glsl linear_to_srgb, no linear toe; Forward+ with sRGB itself), then
## through the tone mapping's inverse, which takes sRGB.
func _light_of(screen: Color) -> float:
	var shown := screen
	if RenderingServer.get_current_rendering_method() == COMPATIBILITY:
		var lin := Color(_gles_decode(screen.r), _gles_decode(screen.g), _gles_decode(screen.b))
		shown = lin.linear_to_srgb()
	return ToneMap.luminance(ToneMap.radiance(shown, world.environment))


static func _gles_decode(e: float) -> float:
	return pow((maxf(e, 0.0) + GLES_OFFSET) / (1.0 + GLES_OFFSET), GLES_POWER)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame
