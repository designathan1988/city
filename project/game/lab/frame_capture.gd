## The frame the player is looking at taken apart (the 4.0's Frame tab, by layer: the engine does not list its draw calls, so each render layer is taken out of the camera for a few
## frames and what the frame loses is what the layer draws, its shadows with it, because the sun's casters follow the camera's mask). Written to a file with a picture of the frame.
class_name FrameCapture
extends Node

signal changed

const FOLDER := "user://lab"
const SETTLE_FRAMES := 4 # what the engine counts lags the frame it drew
const TIMED_FRAMES := 16 # the GPU's time, the median of these
const KINDS := ["objects", "draws", "primitives", "shadow_primitives"]
const SUN_LAYER := "base" # the sun's own: out of the camera it takes every shadow with it, and it holds the previews and nothing else

var rows: Array[Dictionary] = [] # a layer, what it draws, heaviest first
var whole := {}
var file := ""
var busy := false


func take() -> void:
	var camera := get_viewport().get_camera_3d()
	if busy or camera == null:
		return
	busy = true
	changed.emit()
	await _settle()
	whole = _drawn()
	whole.gpu_ms = await _gpu_ms()
	var picture := get_viewport().get_texture().get_image()
	var mask := camera.cull_mask
	var layers: Dictionary = CityStore.constants.render_layers
	rows = []
	for name: String in layers:
		var bit: int = layers[name]
		if (mask & bit) == 0 or name == SUN_LAYER:
			continue
		camera.cull_mask = mask & ~bit
		await _settle()
		var row := _taken_out(name)
		row.gpu_ms = maxf(float(whole.gpu_ms) - await _gpu_ms(), 0.0)
		rows.append(row)
	camera.cull_mask = mask
	rows.sort_custom(func(a: Dictionary, b: Dictionary) -> bool: return a.primitives > b.primitives)
	_keep(picture)
	busy = false
	changed.emit()


func _gpu_ms() -> float:
	var times := PackedFloat32Array()
	for i in TIMED_FRAMES:
		await get_tree().process_frame
		times.append(RenderingServer.viewport_get_measured_render_time_gpu(get_viewport().get_viewport_rid()))
	times.sort()
	return times[TIMED_FRAMES / 2]


func _settle() -> void:
	for i in SETTLE_FRAMES:
		await get_tree().process_frame


func _taken_out(name: String) -> Dictionary:
	var now := _drawn()
	var row := {"layer": name}
	for k: String in KINDS:
		row[k] = maxi(int(whole[k]) - int(now[k]), 0)
	return row


## What the frame draws, what is seen and its shadows together.
func _drawn() -> Dictionary:
	var rs := RenderingServer
	var rid := get_viewport().get_viewport_rid()
	var out := {}
	var kinds := {"objects": rs.VIEWPORT_RENDER_INFO_OBJECTS_IN_FRAME, "draws": rs.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME, "primitives": rs.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME}
	for k: String in kinds:
		out[k] = rs.viewport_get_render_info(rid, rs.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, kinds[k]) + rs.viewport_get_render_info(rid, rs.VIEWPORT_RENDER_INFO_TYPE_SHADOW, kinds[k])
	out.shadow_primitives = rs.viewport_get_render_info(rid, rs.VIEWPORT_RENDER_INFO_TYPE_SHADOW, kinds.primitives)
	return out


func _keep(picture: Image) -> void:
	DirAccess.make_dir_recursive_absolute(FOLDER)
	var stamp := Time.get_datetime_string_from_system().replace(":", "-")
	file = "%s/frame_%s.json" % [FOLDER, stamp]
	var report := {"whole": whole, "layers": rows, "video_mib": RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_VIDEO_MEM_USED) / Units.BYTES_PER_MIB}
	var out := FileAccess.open(file, FileAccess.WRITE)
	if out == null:
		file = ""
		return
	out.store_string(JSON.stringify(report, "\t"))
	picture.save_png(file.trim_suffix(".json") + ".png")
