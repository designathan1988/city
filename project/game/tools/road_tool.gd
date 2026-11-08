## Road tool (Roadcraft editor/roadTool.ts): a road is a drag from its start, or a click that holds the start and a
## second click that lays it. With the Curve alignment a click on the end leaves the bend following the pointer until
## the next click; with Free the road follows the stroke. Where the pointer lands comes from the core (road_end: the
## network, the angles, the grid), and the road in hand from road_pieces; preview and build read the same pieces, so
## what shows green is what gets built. Each road laid ends the gesture. V cycles the alignment, Esc or a right click
## drops it. Its keys are the player's (RoadKeys); by default Page Up and Page Down draw the road a step higher or lower (data/game/tools.json road.height_step_m), landing on whole steps; a road starts at the
## height it was begun at and climbs to the one in hand, and the pointer holds the road at that height, not the ground under it.
extends ToolBase

enum Alignment { STRAIGHT, CURVE, FREE }

const ON_STEP := 0.000001 # a height this near a whole step (in steps) is on it

var alignment: Alignment = Alignment.STRAIGHT
var profile := 0
## The start held by a click, while the next stretch is previewed, and the height it was begun at.
var chain := Vector3.INF
var chain_lift := 0.0
## The height the road in hand is drawn at, over the grade line (the core's: road_height).
var height := 0.0
## The ground under the pointer, the last it was heard of: a change of height moves the road in hand under it.
var pointer := Vector3.INF

## The profile the road is laid with: `profile` under the options of the class (road_variant).
var drawn := 0
## The drag in progress: start, lift (the height it was begun at), chained, pressed (where the press fell), samples, dragged, end (road_end's answer).
var draft: Dictionary = {}
## The curve with its end down, waiting for the click that sets its bend: start, lift, end, control.
var bend: Dictionary = {}
var _last_plan: Dictionary = {}
var _cell := 0.0 # metres; 0: the fine grid of the core
var _cell_offset := 0.0


func enter() -> void:
	profile = params.get("profile", CityStore.first_road())
	drawn = CityStore.dispatch(&"road_variant", [profile, params.get("lanes", 0), params.get("width", 0.0), params.get("parking", 0)])
	if drawn < 0:
		drawn = profile
	_read_cell()
	alignment = params.get("alignment", Alignment.STRAIGHT)
	height = CityStore.query(&"road_height")
	CityStore.dispatch(&"set_road_walls", [Settings.road.walls])
	ctl.hint.emit("", true)


## The grid drawn on the ground holds the road: it fills whole cells, an odd number of them wide with its middle in a cell's middle (roadTool.ts).
func _read_cell() -> void:
	_cell = 0.0
	_cell_offset = 0.0
	if not Settings.layer_on("grid"):
		return
	_cell = DataTables.ui("drawers").road_options.cell_m
	var width: float = CityStore.cached(&"road_profiles")[drawn].width
	_cell_offset = _cell * 0.5 if maxi(1, roundi(width / _cell)) % 2 == 1 else 0.0


func _clear_preview() -> void:
	ctl.preview.clear()
	ctl.tagged.emit("", Vector3.ZERO, true)


func exit() -> void:
	_clear_preview()


## The grid armed in the options: the click lays it, the pointer shows it.
func click(ground: Vector3) -> void:
	if RoadBlocks.settings().armed:
		RoadBlocks.lay(self, ground)
		return
	var p := _aim(ground)
	if not bend.is_empty():
		var b := bend
		bend = {}
		_lay(_pieces(b.start, b.end, true, p, PackedVector3Array()), b.lift)
		return
	var chained := chain != Vector3.INF
	var start: Vector3 = chain if chained else _start_at(p)
	draft = {"start": start, "lift": chain_lift if chained else height, "chained": chained, "pressed": p,
		"samples": PackedVector3Array([start]), "dragged": false, "end": _end_at(start, p)}


func drag(ground: Vector3) -> void:
	var p := _aim(ground)
	if not bend.is_empty():
		_bend_to(p)
	elif not draft.is_empty():
		draft.end = _end_at(draft.start, p)
		RoadDraft.sample(draft, p)
		draft.dragged = draft.dragged or p.distance_to(draft.pressed) > ctl.px_at(p) * DataTables.tuning("road_click_px")
		_preview(_draft_pieces(), draft.lift)


func hover(ground: Vector3) -> void:
	if RoadBlocks.settings().armed:
		RoadBlocks.show(ctl, ground)
		return
	var p := _aim(ground)
	if not bend.is_empty():
		_bend_to(p)
	elif chain != Vector3.INF:
		var end := _end_at(chain, p)
		_preview(_pieces(chain, end.position, false, Vector3.ZERO, PackedVector3Array([chain])), chain_lift)
	else:
		_clear_preview()


## The button let go: a click holds the start, a click on a curve's end waits for its bend, a drag is laid.
func release(ground: Vector3) -> void:
	if draft.is_empty():
		return
	var p := ground if ground == Vector3.INF else _aim(ground)
	var d := draft
	var click_m: float = ctl.px_at(d.pressed) * DataTables.tuning("road_click_px")
	var traveled := RoadDraft.traveled(d)
	var clicked: bool = not d.dragged and (p == Vector3.INF or p.distance_to(d.pressed) < click_m)
	var pieces := _draft_pieces()
	draft = {}
	if not d.chained and (traveled < click_m or clicked):
		chain = d.start
		chain_lift = d.lift
	elif d.chained and alignment == Alignment.CURVE and not d.dragged:
		var e: Vector3 = d.end.position
		bend = {"start": d.start, "lift": d.lift, "end": e, "control": d.start.lerp(e, 0.5)}
	elif traveled >= DataTables.tuning("road_min_len_m"):
		_lay(pieces, d.lift)


func in_progress() -> bool:
	return not draft.is_empty() or not bend.is_empty() or chain != Vector3.INF


func cancel() -> void:
	draft = {}
	bend = {}
	chain = Vector3.INF
	_clear_preview()
	ctl.hint.emit("", true)


func key(keycode: Key) -> void:
	match RoadKeys.action_of(keycode):
		RoadKeys.ALIGNMENT:
			ctl.set_tool(ToolController.Tool.ROAD, params.merged({"alignment": (alignment + 1) % Alignment.size()}, true))
		RoadKeys.RAISE:
			step_height(1)
		RoadKeys.LOWER:
			step_height(-1)


func keeps_key(keycode: Key) -> bool:
	return RoadKeys.action_of(keycode) != ""


## The road a step higher (`dir` 1) or lower (-1), landing on whole steps (roadTool.ts stepHeight); the road in hand goes with it.
func step_height(dir: int) -> void:
	var step: float = DataTables.game("tools").road.height_step_m
	var steps := height / step
	height = ((floorf(steps + ON_STEP) if dir > 0 else ceilf(steps - ON_STEP)) + dir) * step
	CityStore.dispatch(&"set_road_height", [height])
	ctl.road_height_changed.emit()
	if pointer == Vector3.INF:
		return
	if draft.is_empty():
		hover(pointer)
	else:
		drag(pointer)


## Where the pointer stands at the height in hand: along its ray, the ground point seen through the road drawn over it.
func _aim(ground: Vector3) -> Vector3:
	pointer = ground
	var eye: Vector3 = ctl.rig.camera.global_position
	if is_zero_approx(height) or is_zero_approx(ground.y - eye.y):
		return ground
	return eye + (ground - eye) * ((ground.y + height - eye.y) / (ground.y - eye.y))


func _snap() -> Dictionary:
	return Settings.road.snap


func _start_at(p: Vector3) -> Vector3:
	var s := _snap()
	return CityStore.query(&"road_start", [p, ctl.px_at(p), s.on, s.grid, _cell, _cell_offset]).position


func _end_at(start: Vector3, p: Vector3) -> Dictionary:
	var s := _snap()
	return CityStore.query(&"road_end", [start, p, ctl.px_at(p), s.on, s.angles, s.grid, _cell, _cell_offset])


func _draft_pieces() -> PackedVector3Array:
	return _pieces(draft.start, draft.end.position, false, Vector3.ZERO, draft.samples)


func _pieces(a: Vector3, b: Vector3, has_control: bool, control: Vector3, samples: PackedVector3Array) -> PackedVector3Array:
	return CityStore.query(&"road_pieces", [a, b, alignment, has_control, control, samples, drawn, ctl.px_at(b)])


func _bend_to(p: Vector3) -> void:
	bend.control = p
	_preview(_pieces(bend.start, bend.end, true, p, PackedVector3Array()), bend.lift)


func _preview(pieces: PackedVector3Array, from_lift: float) -> void:
	if pieces.is_empty():
		_clear_preview()
		return
	_last_plan = CityStore.dispatch(&"preview_road_path", [pieces, drawn, ctl.px_at(pieces[-1]), from_lift])
	if _last_plan.is_empty():
		_clear_preview()
		return
	var ok: bool = _last_plan.ok and _last_plan.affordable
	ctl.preview.show_road(_last_plan, RoadDraft.paint(drawn), ok, ctl.px_at(pieces[0]), ctl.px_at(pieces[-1]))
	var text := RoadDraft.measure(_last_plan)
	if not _last_plan.ok:
		text += " · " + reason(_last_plan)
	elif not _last_plan.affordable:
		text += " · " + tr("rule.short.funds")
	ctl.tagged.emit(text, pieces[-1], ok)


func _lay(pieces: PackedVector3Array, from_lift: float) -> void:
	var laid_plan := _last_plan
	_clear_preview()
	if pieces.is_empty():
		return
	var r: Dictionary = CityStore.dispatch(&"build_road_path", [pieces, drawn, ctl.px_at(pieces[-1]), from_lift])
	if r.is_empty() or not r.ok:
		ctl.hint.emit(refusal(r), false)
		return
	chain = Vector3.INF
	if laid_plan.has("points"):
		ctl.flash.flash(laid_plan.points, laid_plan.half_width)
	ctl.hint.emit(Formatters.format_cost(r.money), true)
