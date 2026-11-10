## The F7 checks (the 4.0's lab/resources/diagnostics): the machine and the game's resources one by one, each with its verdict. What grows while the
## panel is open is read as a slope over the window, a line through the readings: one reading up is a load, a rising line is a leak.
class_name ResourceWatch
extends RefCounted

const MIB := 1048576.0
const S_PER_MIN := 60.0
## The monitors that must not climb, in MiB.
const MEMORY := {"static": Performance.MEMORY_STATIC, "video": Performance.RENDER_VIDEO_MEM_USED, "texture": Performance.RENDER_TEXTURE_MEM_USED, "buffer": Performance.RENDER_BUFFER_MEM_USED}
const COUNTS := {"objects": Performance.OBJECT_COUNT, "nodes": Performance.OBJECT_NODE_COUNT}

var _points: Array[Dictionary] = []


func sample(now_s: float) -> void:
	var p := {"t": now_s, "pipelines": float(FrameMeter.pipelines())}
	for key: String in MEMORY:
		p[key] = Performance.get_monitor(MEMORY[key]) / MIB
	for key: String in COUNTS:
		p[key] = Performance.get_monitor(COUNTS[key])
	_points.append(p)
	var window: float = DataTables.game("lab").resources.window_s
	while _points.size() > 1 and now_s - float(_points[0].t) > window:
		_points.pop_front()


## Units a minute: the least-squares line through (seconds, value).
static func slope(points: Array[Dictionary], key: String) -> float:
	var n := float(points.size())
	if n < 2.0:
		return 0.0
	var sx := 0.0
	var sy := 0.0
	var sxx := 0.0
	var sxy := 0.0
	for p: Dictionary in points:
		sx += p.t
		sy += p[key]
		sxx += p.t * p.t
		sxy += p.t * p[key]
	var d := n * sxx - sx * sx
	return 0.0 if is_zero_approx(d) else (n * sxy - sx * sy) / d * S_PER_MIN


func _seen_s() -> float:
	return 0.0 if _points.size() < 2 else float(_points[-1].t) - float(_points[0].t)


## Every check now: {id, verdict (ok, info, warning, problem), value}.
func checks() -> Array[Dictionary]:
	var rules: Dictionary = DataTables.game("lab").resources
	var out: Array[Dictionary] = []
	var software := RenderingServer.get_video_adapter_type() == RenderingDevice.DEVICE_TYPE_CPU
	out.append({"id": "adapter", "verdict": "problem" if software else "ok", "value": RenderingServer.get_video_adapter_name()})
	out.append({"id": "method", "verdict": "ok" if RenderingServer.get_current_rendering_method() == rules.best_method else "info", "value": RenderingServer.get_current_rendering_method()})
	var enough := _seen_s() >= float(rules.least_s)
	for key: String in MEMORY:
		var up := slope(_points, key)
		out.append({"id": key, "verdict": "warning" if enough and up > float(rules.grow_mib_min) else "ok" if enough else "info", "value": "%+.1f MiB/min" % up})
	for key: String in COUNTS:
		var up := slope(_points, key)
		out.append({"id": key, "verdict": "warning" if enough and up > float(rules.grow_count_min) else "ok" if enough else "info", "value": "%+.0f/min" % up})
	var orphans := int(Performance.get_monitor(Performance.OBJECT_ORPHAN_NODE_COUNT))
	out.append({"id": "orphans", "verdict": "warning" if orphans > 0 else "ok", "value": str(orphans)})
	var made := 0.0 if _points.is_empty() else float(_points[-1].pipelines) - float(_points[0].pipelines)
	out.append({"id": "pipelines", "verdict": "warning" if made > 0.0 else "ok", "value": str(int(made))})
	return out
