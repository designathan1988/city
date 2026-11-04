## Road tool (Roadcraft editor/roadTool.ts): a road is a drag from its start, or a click that holds the start and a
## second click that lays it. With the Curve alignment a click on the end leaves the bend following the pointer until
## the next click; with Free the road follows the stroke. Where the pointer lands comes from the core (road_end: the
## network, the angles, the grid), and the road in hand from road_pieces; preview and build read the same pieces, so
## what shows green is what gets built. Each road laid ends the gesture. V cycles the alignment, Esc or a right click
## drops it.
extends ToolBase

enum Alignment { STRAIGHT, CURVE, FREE }

## A long stroke keeps every sample up to DENSE_SAMPLES, then one a metre, up to MOST_SAMPLES.
const DENSE_SAMPLES := 256
const MOST_SAMPLES := 4096
const SPARSE_SAMPLE_M := 1.0

var alignment: Alignment = Alignment.STRAIGHT
var profile := 0
## The start held by a click, while the next stretch is previewed.
var chain := Vector3.INF
## The drag in progress: start, chained, pressed (where the press fell), samples, dragged, end (road_end's answer).
var draft: Dictionary = {}
## The curve with its end down, waiting for the click that sets its bend: start, end, control.
var bend: Dictionary = {}
var _last_plan: Dictionary = {}


func enter() -> void:
	profile = params.get("profile", CityStore.first_road())
	alignment = params.get("alignment", Alignment.STRAIGHT)
	ctl.hint.emit(tr("ui.do.roadPoint"), true)


func exit() -> void:
	ctl.preview.clear()


func click(p: Vector3) -> void:
	if not bend.is_empty():
		var b := bend
		bend = {}
		_lay(_pieces(b.start, b.end, true, p, PackedVector3Array()))
		return
	var chained := chain != Vector3.INF
	var start: Vector3 = chain if chained else _start_at(p)
	draft = {"start": start, "chained": chained, "pressed": p, "samples": PackedVector3Array([start]), "dragged": false,
		"end": _end_at(start, p)}


func drag(p: Vector3) -> void:
	if not bend.is_empty():
		_bend_to(p)
	elif not draft.is_empty():
		draft.end = _end_at(draft.start, p)
		_sample(p)
		draft.dragged = draft.dragged or p.distance_to(draft.pressed) > ctl.px_at(p) * DataTables.tuning("road_click_px")
		_preview(_draft_pieces(), draft.end)


func hover(p: Vector3) -> void:
	if not bend.is_empty():
		_bend_to(p)
	elif chain != Vector3.INF:
		var end := _end_at(chain, p)
		_preview(_pieces(chain, end.position, false, Vector3.ZERO, PackedVector3Array([chain])), end)
	else:
		ctl.preview.clear()


## The button let go: a click holds the start, a click on a curve's end waits for its bend, a drag is laid.
func release(p: Vector3) -> void:
	if draft.is_empty():
		return
	var d := draft
	var click_m: float = ctl.px_at(d.pressed) * DataTables.tuning("road_click_px")
	var traveled := _traveled(d)
	var clicked: bool = not d.dragged and (p == Vector3.INF or p.distance_to(d.pressed) < click_m)
	var pieces := _draft_pieces()
	draft = {}
	if not d.chained and (traveled < click_m or clicked):
		chain = d.start
	elif d.chained and alignment == Alignment.CURVE and not d.dragged:
		var e: Vector3 = d.end.position
		bend = {"start": d.start, "end": e, "control": d.start.lerp(e, 0.5)}
	elif traveled >= DataTables.tuning("road_min_len_m"):
		_lay(pieces)


func cancel() -> void:
	draft = {}
	bend = {}
	chain = Vector3.INF
	ctl.preview.clear()
	ctl.hint.emit(tr("ui.do.roadPoint"), true)


func key(keycode: Key) -> void:
	if keycode == KEY_V:
		ctl.set_tool(ToolController.Tool.ROAD, params.merged({"alignment": (alignment + 1) % Alignment.size()}, true))


func _snap() -> Dictionary:
	return Settings.road_snap


func _start_at(p: Vector3) -> Vector3:
	var s := _snap()
	return CityStore.query(&"road_start", [p, ctl.px_at(p), s.on, s.grid]).position


func _end_at(start: Vector3, p: Vector3) -> Dictionary:
	var s := _snap()
	return CityStore.query(&"road_end", [start, p, ctl.px_at(p), s.on, s.angles, s.grid])


func _sample(p: Vector3) -> void:
	var samples: PackedVector3Array = draft.samples
	if samples.size() >= MOST_SAMPLES:
		return
	if samples.size() >= DENSE_SAMPLES and p.distance_to(samples[-1]) < SPARSE_SAMPLE_M:
		return
	samples.push_back(p)
	draft.samples = samples


func _traveled(d: Dictionary) -> float:
	var total := 0.0
	var samples: PackedVector3Array = d.samples
	for i in range(1, samples.size()):
		total += samples[i].distance_to(samples[i - 1])
	return total + samples[-1].distance_to(d.end.position)


func _draft_pieces() -> PackedVector3Array:
	return _pieces(draft.start, draft.end.position, false, Vector3.ZERO, draft.samples)


func _pieces(a: Vector3, b: Vector3, has_control: bool, control: Vector3, samples: PackedVector3Array) -> PackedVector3Array:
	return CityStore.query(&"road_pieces", [a, b, alignment, has_control, control, samples, profile, ctl.px_at(b)])


func _bend_to(p: Vector3) -> void:
	bend.control = p
	_preview(_pieces(bend.start, bend.end, true, p, PackedVector3Array()), {})


func _preview(pieces: PackedVector3Array, end: Dictionary) -> void:
	if pieces.is_empty():
		ctl.preview.clear()
		return
	_last_plan = CityStore.dispatch(&"preview_road_path", [pieces, profile, ctl.px_at(pieces[-1])])
	if _last_plan.is_empty():
		ctl.preview.clear()
		return
	var ok: bool = _last_plan.ok and _last_plan.affordable
	ctl.preview.show_ribbon(_last_plan.points, _last_plan.half_width, ok)
	if not _last_plan.ok:
		ctl.hint.emit(refusal(_last_plan), false)
	elif not _last_plan.affordable:
		ctl.hint.emit(refusal({"error": "ui.insp.noMoney"}, _last_plan.price), false)
	else:
		ctl.hint.emit(_measure(end), true)


## "120 m · R$ 150.000 · 90°": the angle only when something snapped it.
func _measure(end: Dictionary) -> String:
	var text := "%d m · %s" % [roundi(_last_plan.length), Formatters.format_money(_last_plan.price)]
	return text + (" · %d°" % roundi(end.angle_deg) if end.get("guide", "") != "" else "")


func _lay(pieces: PackedVector3Array) -> void:
	ctl.preview.clear()
	if pieces.is_empty():
		return
	var r: Dictionary = CityStore.dispatch(&"build_road_path", [pieces, profile, ctl.px_at(pieces[-1])])
	if r.is_empty() or not r.ok:
		ctl.hint.emit(refusal(r, _last_plan.get("price", 0.0)), false)
		return
	chain = Vector3.INF
	ctl.hint.emit(Formatters.format_money(r.money), true)
