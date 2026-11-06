## The CPU tab of the F7 panel (the 4.0's perfil de CPU, by the core's zones and not by function: tools/analysis/sampler.py reads the functions): a window of
## frames of what each zone of the frame took, the mean, the tail and the worst, since a single frame says nothing of what a frame usually costs.
class_name CpuWatch
extends RefCounted

const PERCENT := 100.0
const DRAW := "draw"
const SETUP := "setup"

var _frames := {} # zone -> its last times, ms
var _zones: Array[String] = []
var _window := 0
var _at := 0
var _full := false


func _init(zones: Array[String], window: int) -> void:
	_zones = zones.duplicate()
	_zones.append_array([DRAW, SETUP])
	_window = window
	for key: String in _zones:
		var times := PackedFloat32Array()
		times.resize(window)
		_frames[key] = times


## One frame's reading.
func sample(viewport: RID) -> void:
	var cost: Dictionary = CityStore.query(&"render_cost")
	cost[DRAW] = RenderingServer.viewport_get_measured_render_time_cpu(viewport)
	cost[SETUP] = RenderingServer.get_frame_setup_time_cpu()
	for key: String in _zones:
		var times: PackedFloat32Array = _frames[key]
		times[_at] = float(cost.get(key, 0.0))
		_frames[key] = times
	_at += 1
	if _at == _window:
		_at = 0
		_full = true


func frames() -> int:
	return _window if _full else _at


## The zones by their mean, heaviest first: {key, mean, tail, worst, share of the sum of the means}.
func rows(tail: float) -> Array[Dictionary]:
	var out: Array[Dictionary] = []
	var count := frames()
	if count == 0:
		return out
	var total := 0.0
	for key: String in _zones:
		var times: PackedFloat32Array = _frames[key].slice(0, count)
		times.sort()
		var sum := 0.0
		for t in times:
			sum += t
		total += sum / count
		out.append({"key": key, "mean": sum / count, "tail": times[mini(count - 1, int(count * tail))], "worst": times[count - 1]})
	for r: Dictionary in out:
		r.share = r.mean / total * PERCENT if total > 0.0 else 0.0
	out.sort_custom(func(a: Dictionary, b: Dictionary) -> bool: return a.mean > b.mean)
	return out
