## The one refusal channel, with the HUD up and a map open: a refused tool command (the bulldozer on empty ground) and a
## refused panel command (the road panel joining two roads of two classes) are told once, by CityStore.command_failed, on the
## hint bar in its danger style with Refusals.text; a preview's refusal leaves the bar as it was; a command done tells nothing;
## an answer done in part (ok, with an error) is told, a no-op (ok, with only a note) is not; a lot clicked with its own zone
## says so plainly; a grid of streets laid says what was built, with its numbers.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const OUT_M := 460.0 # open ground past the town (road_edit_test.gd)
const PX := 0.25 # metres a screen pixel covers: a close view (road_edit_test.gd)
const EMPTY_OFF_M := 120.0 # ground with nothing on it, off the roads this test lays
const JOIN_OFF_M := 300.0
const ROAD_M := 60.0
const SHORT_OFF_M := 480.0
const SHORT_SHARE := 0.5 # of the shortest road a plan may lay: a preview this long is refused
const FIRST_CLASS := "localStreet"
const SECOND_CLASS := "urbanStreet"
const ZONE_OFF_M := 660.0
const ZONE_BRUSH_M := 30.0 # camera_road_key_cases.gd
const ZONE_KIND := 1
const LOT_OFF_M := 780.0
const LOT_M := 40.0
const BLOCKS_OUT_M := 700.0 # east of the other cases: the grid (data/game/settings.json road_blocks) meets none of them

var _failed: Array = [] # [command, result] of each command_failed heard


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var tools := ToolController.new(rig)
	add_child(tools)
	var hud := RoadcraftHud.new()
	add_child(hud)
	CityStore.command_failed.connect(_on_failed)
	var b: Dictionary = CityStore.query(&"city_bounds")
	var e := Vector3(b.max.x + OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	_tool_refusal(hud, tools, e)
	_panel_refusal(hud, e)
	_preview_refusal(hud, e)
	_partial(hud)
	_no_op(hud, e)
	_zoned_lot(hud, tools, e)
	_blocks(hud, tools, e)
	CityStore.command_failed.disconnect(_on_failed)
	finish()


func _on_failed(command: StringName, result: Dictionary) -> void:
	_failed.append([command, result])


## What the hint bar says now: its text, its ink, whether it shows.
func _bar(hud: RoadcraftHud) -> Array:
	var label: Label = hud.messages._hint
	return [label.text, label.theme_type_variation, label.get_parent().visible]


## (a) The bulldozer released on empty ground, through the tool's own path: the refusal is told once, by the channel alone.
func _tool_refusal(hud: RoadcraftHud, tools: ToolController, e: Vector3) -> void:
	tools.set_tool(ToolController.Tool.DEMOLISH)
	var told: Array[String] = []
	var keep := func(text: String, _ok: bool) -> void: told.append(text)
	tools.hint.connect(keep)
	var empty := e + Vector3(0, 0, EMPTY_OFF_M)
	var before := _failed.size()
	tools.active.click(empty)
	tools.active.release(empty)
	tools.hint.disconnect(keep)
	if not check(_failed.size() == before + 1, "a refused demolition is told once by the channel: %s" % [_failed.slice(before)]):
		return
	check(_failed[-1][0] == &"demolish", "the channel names the command: %s" % [_failed[-1][0]])
	var text := Refusals.text(_failed[-1][1])
	check(not text.is_empty() and _bar(hud) == [text, &"DangerLabel", true], "the hint bar shows it in the danger style: %s" % [_bar(hud)])
	check(not told.has(text), "the tool tells it no second time: %s" % [told])


## (b) The road panel's Join on a point where two roads of two classes meet in line: the core refuses, the bar says why.
func _panel_refusal(hud: RoadcraftHud, e: Vector3) -> void:
	var a := e + Vector3(0, 0, JOIN_OFF_M)
	var mid := a + Vector3(ROAD_M, 0, 0)
	var before := _failed.size()
	var laid := _lay(a, mid, FIRST_CLASS) and _lay(mid, mid + Vector3(ROAD_M, 0, 0), SECOND_CLASS)
	if not check(laid, "two roads of two classes meet in line"):
		return
	check(_failed.size() == before, "a command done tells the channel nothing: %s" % [_failed.slice(before)])
	var info: Dictionary = CityStore.query(&"inspect", [mid])
	if not check(info.get("kind", "") == "node" and int(info.get("arms", 0)) == 2, "where they meet is a point of two arms: %s" % [info]):
		return
	hud.show_hint("", true)
	var col := VBoxContainer.new()
	hud.root.add_child(col)
	RoadPanel.point(col, info, func() -> void: pass, func() -> void: pass)
	var join := _button(col, RoadPanel.tr_key("inspector.joinAligned"))
	if check(join != null, "the panel offers to join them"):
		before = _failed.size()
		join.pressed.emit()
		if check(_failed.size() == before + 1 and _failed[-1][0] == &"join_roads", "the refused join is told once: %s" % [_failed.slice(before)]):
			var text := Refusals.text(_failed[-1][1])
			check(not text.is_empty() and _bar(hud) == [text, &"DangerLabel", true], "the hint bar shows the panel's refusal: %s" % [_bar(hud)])
	col.queue_free()


## (c) A preview the core refuses (a road shorter than the shortest it lays): the channel hears nothing, the bar stays.
func _preview_refusal(hud: RoadcraftHud, e: Vector3) -> void:
	hud.show_hint(tr("hint.control"), true)
	var was := _bar(hud)
	var a := e + Vector3(0, 0, SHORT_OFF_M)
	var b := a + Vector3(DataTables.tuning("road_min_len_m") * SHORT_SHARE, 0, 0)
	var before := _failed.size()
	var plan: Dictionary = CityStore.dispatch(&"preview_road", [a, b, false, Vector3.ZERO, _class(FIRST_CLASS), PX, 0.0])
	if not check(not plan.get("ok", true) and not str(plan.get("error", "")).is_empty(), "a road that short is refused in preview: %s" % [plan]):
		return
	check(_failed.size() == before, "a preview's refusal is not told: %s" % [_failed.slice(before)])
	check(_bar(hud) == was, "the hint bar stays as it was: %s, was %s" % [_bar(hud), was])


## (e) More clouds asked than the sky holds: some laid, the rest refused ({ok: true, error}): told once, on the bar.
func _partial(hud: RoadcraftHud) -> void:
	var c: Dictionary = DataTables.catalog_table("nature").clouds
	var like := {"height": c.height_m.default, "size": c.size_m.default, "density": c.density.default}
	hud.show_hint("", true)
	var before := _failed.size()
	var r: Dictionary = CityStore.dispatch(&"scatter_clouds", [int(c.max_clouds) + 1, like, 0.0])
	if check(r.get("ok", false) and not str(r.get("error", "")).is_empty(), "more clouds than the sky holds is done in part: %s" % [r]):
		check(_failed.size() == before + 1 and _failed[-1][0] == &"scatter_clouds", "the part refused is told once: %s" % [_failed.slice(before)])
		check(_bar(hud) == [Refusals.text(r), &"DangerLabel", true], "the hint bar shows the part refused: %s" % [_bar(hud)])
	CityStore.dispatch(&"clear_clouds")


## (f) A road side zoned again with its own zone: a no-op ({ok: true, note}, no error): the channel hears nothing, the bar stays.
func _no_op(hud: RoadcraftHud, e: Vector3) -> void:
	var a := e + Vector3(0, 0, ZONE_OFF_M)
	var mid := a + Vector3(ROAD_M * 0.5, 0, 0)
	if not _lay(a, a + Vector3(ROAD_M, 0, 0), FIRST_CLASS):
		return
	var first: Dictionary = CityStore.dispatch(&"paint_zone", [mid, ZONE_BRUSH_M, ZONE_KIND])
	if not check(first.get("ok", false) and int(first.get("zoned_sides", 0)) > 0, "the road's sides take the zone: %s" % [first]):
		return
	hud.show_hint(tr("hint.control"), true)
	var was := _bar(hud)
	var before := _failed.size()
	var again: Dictionary = CityStore.dispatch(&"paint_zone", [mid, ZONE_BRUSH_M, ZONE_KIND])
	var no_op: bool = again.get("ok", false) and str(again.get("error", "")).is_empty() and not str(again.get("note", "")).is_empty()
	if check(no_op, "the same zone again is a no-op with a note: %s" % [again]):
		check(_failed.size() == before, "a no-op is not told: %s" % [_failed.slice(before)])
		check(_bar(hud) == was, "the hint bar stays as it was: %s, was %s" % [_bar(hud), was])


## (g) The zone tool clicked on a player's lot that has its zone already: the core's note as a plain hint, nothing told.
func _zoned_lot(hud: RoadcraftHud, tools: ToolController, e: Vector3) -> void:
	var at := e + Vector3(0, 0, LOT_OFF_M)
	var mid := at + Vector3(LOT_M * 0.5, 0, LOT_M * 0.5)
	var drawn: Dictionary = CityStore.dispatch(&"lot_add_rect", [at, at + Vector3(LOT_M, 0, LOT_M), 0.0])
	var id: int = CityStore.query(&"lot_at", [mid])
	var zoned: Dictionary = CityStore.dispatch(&"lot_zone", [id, ZONE_KIND]) if id != 0 else {}
	if not check(drawn.get("ok", false) and zoned.get("ok", false), "a lot drawn and zoned: %s %s" % [drawn, zoned]):
		return
	tools.set_tool(ToolController.Tool.ZONE, {"zone": ZONE_KIND, "radius": ZONE_BRUSH_M})
	var before := _failed.size()
	tools.active.click(mid)
	tools.active.release(mid)
	check(_failed.size() == before, "zoning a lot with its own zone is not told: %s" % [_failed.slice(before)])
	check(_bar(hud) == [tr("tools.toast.zoneSame"), &"BodyLabel", true], "the lot clicked says so plainly: %s" % [_bar(hud)])


## (h) A grid of streets laid by the road tool's click: the success text with its numbers filled in, nothing told.
func _blocks(hud: RoadcraftHud, tools: ToolController, e: Vector3) -> void:
	tools.set_tool(ToolController.Tool.ROAD)
	var grid := RoadBlocks.settings()
	grid.armed = true
	var built := String(TranslationServer.translate("tools.toast.blocksBuilt")).format({"cols": int(grid.cols), "rows": int(grid.rows)})
	var segments: int = CityStore.snapshot.segments
	var before := _failed.size()
	tools.active.click(e + Vector3(BLOCKS_OUT_M, 0, 0))
	var laid: bool = CityStore.snapshot.segments > segments and _failed.size() == before
	if check(laid, "the grid is laid: %s" % [_failed.slice(before)]):
		check(not built.contains("{") and _bar(hud) == [built, &"BodyLabel", true], "the bar says what was built: %s" % [_bar(hud)])


func _lay(a: Vector3, b: Vector3, id: String) -> bool:
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, _class(id), PX, 0.0])
	return check(r.get("ok", false), "road %s laid: %s" % [id, r])


## The index of the road class `id` (data/catalog/road_profiles.json).
func _class(id: String) -> int:
	for p: Dictionary in CityStore.cached(&"road_profiles"):
		if p.id == id:
			return p.index
	return -1


func _button(col: Control, text: String) -> Button:
	for b: Node in col.find_children("*", "Button", true, false):
		if (b as Button).text == text:
			return b
	return null
