## Public transport tool: `params.kind` says what a click lays. stop, terminal and station act at once; track and line collect points
## or stops and Enter ends them, Backspace takes the last back, Esc drops them. Shift and a click removes the stop or track there. The
## core checks everything: refusals come back as hint keys.
extends ToolBase

const MODE_ANY := 0
const MODE_TRAIN := 1
const TRACK_HALF_M := 1.9 # the ballast, as data/render/transit.json draws it

var kind := "stop"
var mode := MODE_TRAIN
var points := PackedVector3Array()
var stops := PackedInt32Array()


func enter() -> void:
	kind = params.get("kind", "stop")
	mode = params.get("mode", MODE_TRAIN)
	ctl.hint.emit(tr("game.hint.transit"), true)


func exit() -> void:
	ctl.preview.clear()


func click(p: Vector3) -> void:
	if Input.is_key_pressed(KEY_SHIFT):
		_say(CityStore.dispatch(&"transit_remove", [p]))
		return
	match kind:
		"stop":
			_say(CityStore.dispatch(&"transit_stop", [p]))
		"terminal":
			_say(CityStore.dispatch(&"transit_terminal", [p]))
		"station":
			_say(CityStore.dispatch(&"transit_station", [p, mode]))
		"track":
			points.append(CityStore.query(&"transit_snap", [p, mode]))
			ctl.preview.show_ribbon(points, TRACK_HALF_M, true)
		"line":
			var stop: int = CityStore.query(&"transit_pick", [p, MODE_ANY, true])
			if stop < 0:
				ctl.hint.emit(tr("hint.transit.noStop"), false)
			elif stops.is_empty() or stops[stops.size() - 1] != stop:
				stops.append(stop)


func hover(p: Vector3) -> void:
	if kind == "track" and not points.is_empty():
		var run := points.duplicate()
		run.append(CityStore.query(&"transit_snap", [p, mode]))
		ctl.preview.show_ribbon(run, TRACK_HALF_M, true)


func cancel() -> void:
	points.clear()
	stops.clear()
	ctl.preview.clear()


func key(code: int) -> void:
	if points.is_empty() and stops.is_empty():
		return
	if code == KEY_ENTER or code == KEY_KP_ENTER:
		finish()
	elif code == KEY_BACKSPACE:
		if not points.is_empty():
			points.resize(points.size() - 1)
		if not stops.is_empty():
			stops.resize(stops.size() - 1)


## Lays the run or makes the line collected so far.
func finish() -> void:
	if kind == "track":
		_say(CityStore.dispatch(&"transit_track", [points, mode]))
	elif kind == "line":
		_say(CityStore.dispatch(&"transit_line", [stops]))
	cancel()


func _say(result: Dictionary) -> void:
	if not result.get("ok", false):
		ctl.hint.emit(tr(result.get("error", "")), false)
