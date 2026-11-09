## The Build drawer's Selection panel (the Roadcraft's Builder inspector): the precise numbers of what the Build tool picked (what the
## building is for, its heights and the picked level's own, the block's size, place and levels, the roof's pitch, ridge and fall, the
## face's proportions, a free part's size, a roof thing's height), each typed, stepped or chosen as one edit. Holds no number of the game: it asks the core (body_fields) and sends the
## edit (CityStore.dispatch); what it shows comes back from the core after each change. Fields and ranges are data/ui/builder.json's.
class_name BuilderInspector
extends VBoxContainer

## A unit's text (strings.csv, "{n}" left empty) and how many of it a stored number is.
const UNIT_KEYS := {"m": "unit.metre", "deg": "unit.degree", "share": "unit.percent", "count": ""}
const PITCHED_FROM := &"ROOF_GABLE" # Roof codes from this one on slope

@export var tools: ToolController

var _picked := {}
var _fields := {}
var _stale := true
var _grid: GridContainer # the fields of the pick, a cell each (a name over its control), as many to a row as builder.json says


static func fill(col: Container, controller: ToolController) -> void:
	var panel := BuilderInspector.new()
	panel.tools = controller
	col.add_child(panel)


func _ready() -> void:
	CityStore.changed.connect(func(_snapshot: Dictionary) -> void: _stale = true)


## What the tool picked, read each frame (a click changes it); the numbers asked again only when the pick or the city changed.
func _process(_delta: float) -> void:
	var tool := tools.active as BuildingTool
	var picked: Dictionary = tool.picked if tool != null else {}
	if picked == _picked and not _stale:
		return
	_stale = false
	var fields: Dictionary = {} if picked.is_empty() else CityStore.query(&"body_fields", [picked.id, picked.volume, picked.side, picked.get("bay", -1), picked.storey, picked.get("part", -1), picked.get("gear", -1)])
	if picked == _picked and fields == _fields:
		return
	var body_changed := picked == _picked and tool != null
	_picked = picked.duplicate()
	_fields = fields
	_build()
	if body_changed: # the same pick, other numbers: the body changed under it (an edit, an undo), so the tool draws its box again
		tool.show_pick()


func _build() -> void:
	HudWidgets.clear(self)
	if _fields.is_empty():
		HudWidgets.label(self, &"MutedLabel", tr("hint.builder.select"))
		return
	HudWidgets.label(self, &"FieldLabel", _name())
	_grid = GridContainer.new()
	_grid.columns = int(DataTables.ui("builder").selection_columns)
	add_child(_grid)
	for spec: Dictionary in DataTables.ui("builder").fields:
		var shown: Variant = _value(spec)
		if shown != null and spec.unit == "choice":
			_function_field(spec, int(shown))
		elif shown != null:
			_row(spec, float(shown))
	var block: Dictionary = _fields.get("block", {})
	if not block.is_empty() and _pitched(block) and _picked.get("part", -1) < 0 and _picked.get("gear", -1) < 0:
		_roof_rows(block, HudWidgets.hbox(self, &"Gap"))


## What the panel is looking at, in one line.
func _name() -> String:
	if _fields.has("part"):
		return tr("builder.sel.element")
	if _fields.has("gear"):
		return tr("builder.sel.detail")
	if _fields.has("face") and _picked.get("bay", -1) >= 0:
		return "%s · %s" % [tr("builder.sel.face"), tr("builder.sel.floor").format({"n": int(_picked.level) + 1})]
	return tr("builder.sel.volume").format({"n": int(_picked.volume) + 1})


## The number a field shows for the pick, or null when the pick has none: a part or a roof thing shows only its own.
func _value(spec: Dictionary) -> Variant:
	var thing := _fields.has("part") or _fields.has("gear")
	var block: Dictionary = _fields.get("block", {})
	match spec.of:
		"building", "record":
			return null if thing else _fields.get(spec.key)
		"block", "level":
			return null if thing else block.get(spec.key)
		"offset":
			return null if thing or not block.has("offset") else (block.offset as Vector2)[0 if spec.key == "x" else 1]
		"pitched":
			return block.get(spec.key) if not thing and not block.is_empty() and _pitched(block) else null
		"bay":
			return null if thing else (_fields.get("face", {}) as Dictionary).get(spec.key)
		"face":
			return null if thing else (_fields.get("face", {}) as Dictionary).get(spec.key)
		"part", "gear":
			return (_fields.get(spec.of, {}) as Dictionary).get(spec.key)
	return null


func _pitched(block: Dictionary) -> bool:
	return int(block.roof) >= CityStore.code(&"Roof", PITCHED_FROM)


func _bound(v: Variant) -> float:
	if v is String:
		var name: String = v
		return -DataTables.tuning(name.substr(1)) if name.begins_with("-") else DataTables.tuning(name)
	return float(v)


## A cell of the grid: the field's name over its control, which is added to what this returns.
func _cell(title: String) -> VBoxContainer:
	var cell := HudWidgets.vbox(_grid)
	HudWidgets.label(cell, &"MutedLabel", title)
	return cell


func _row(spec: Dictionary, value: float) -> void:
	var row := _cell(tr(spec.label_key))
	var share: bool = spec.unit == "share"
	var k := Units.PERCENT if share else 1.0
	var box := SpinBox.new()
	box.min_value = _bound(spec.lo) * k
	box.max_value = _bound(spec.hi) * k
	box.step = float(spec.step) * k
	box.value = value * k
	var unit_key: String = UNIT_KEYS[spec.unit]
	box.suffix = tr(unit_key).format({"n": ""}).strip_edges() if unit_key != "" else ""
	box.custom_minimum_size.x = HudWidgets.px("slider_w")
	row.add_child(box)
	box.value_changed.connect(func(v: float) -> void: _send(spec, v / k))


## What the building is for (data/ui/builder.json functions): any building may become any of them, its body kept.
func _function_field(spec: Dictionary, fn: int) -> void:
	var list := OptionButton.new()
	for f: Dictionary in DataTables.ui("builder").functions:
		var code := CityStore.code(&"BuildingFn", StringName(f.fn))
		list.add_item(tr(f.label_key), code)
		if code == fn:
			list.select(list.item_count - 1)
	_cell(tr(spec.label_key)).add_child(list)
	list.item_selected.connect(func(i: int) -> void: _send(spec, float(list.get_item_id(i))))


## A typed number as its edit (the commands of the Build tab, CityStore StoreApi).
func _send(spec: Dictionary, v: float) -> void:
	var id: int = _picked.id
	var vol: int = _picked.volume
	var enums: Dictionary = CityStore.constants.enums
	var r: Dictionary = {}
	match spec.id:
		"function":
			r = CityStore.dispatch(&"building_function", [id, roundi(v)])
		"levelHeight":
			r = CityStore.dispatch(&"body_level_height", [id, _fields.block.level, v])
		"storeys":
			r = CityStore.dispatch(&"body_storeys", [id, vol, roundi(v)])
		"storeyHeight":
			r = CityStore.dispatch(&"body_heights", [id, _fields.ground_h, v])
		"groundHeight":
			r = CityStore.dispatch(&"body_heights", [id, v, _fields.storey_h])
		"width", "depth":
			r = CityStore.dispatch(&"body_block_size", [id, vol, enums.PlanAxis.AXIS_X if spec.id == "width" else enums.PlanAxis.AXIS_Y, v])
		"offsetX", "offsetY":
			r = CityStore.dispatch(&"body_block_offset", [id, vol, enums.PlanAxis.AXIS_X if spec.id == "offsetX" else enums.PlanAxis.AXIS_Y, v])
		"baseLevel":
			r = CityStore.dispatch(&"body_block_base", [id, vol, roundi(v)])
		"lift":
			r = CityStore.dispatch(&"body_apply", [id, {"op": "lift", "vol": vol, "along": v, "snap": false}])
		"pitch":
			r = CityStore.dispatch(&"body_roof_shape", [id, vol, v, _fields.block.ridge, _fields.block.fall])
		"relief":
			var region := {"side": _picked.side, "bay0": _picked.bay, "bay1": _picked.bay, "storey0": _picked.storey, "storey1": _picked.storey, "depth": v}
			r = CityStore.dispatch(&"body_set_relief", [id, vol, region, false])
		"partWidth", "partLength", "partHeight":
			var axis: int = {"partWidth": enums.PartAxis.PART_ACROSS, "partLength": enums.PartAxis.PART_ALONG, "partHeight": enums.PartAxis.PART_HEIGHT}[spec.id]
			r = CityStore.dispatch(&"body_size_part", [id, _picked.part, axis, v])
		"gearHeight":
			r = CityStore.dispatch(&"body_size_gear", [id, vol, _picked.gear, v])
		_:
			r = CityStore.dispatch(&"body_set_face", [id, vol, _picked.side, {spec.key: roundi(v) if spec.unit == "count" else v}])
	if not r.get("ok", false):
		tools.hint.emit(tools.active.refusal(r) if tools.active != null else tr(r.get("error", "tool.failed")), false)
	_stale = true


## The roof's slope, in a row of its own under the grid (its choices are wider than a number's cell): the pitch a step at a time, the
## ridge's way and the side a single slope falls to (the Roadcraft's roof shelf).
func _roof_rows(block: Dictionary, row: HBoxContainer) -> void:
	var choices: Dictionary = DataTables.ui("builder").roof_choices
	var step: float = choices.pitch_step_deg
	var pitch := HudWidgets.hbox(_group(row, tr("builder.section.roofShape")))
	for sign: float in [-1.0, 1.0]:
		var b := HudWidgets.button(&"Card", "b_storeyDown" if sign < 0 else "b_storey", "", "builder.field.pitch", "icon_btn", "tab_h")
		b.pressed.connect(func() -> void: _shape(float(block.pitch) + sign * step, block.ridge, block.fall))
		pitch.add_child(b)
	_choice_row(_group(row, tr("builder.roof.ridge.label")), choices.ridge, 0 if bool(block.ridge_x) else 1,
		func(v: int) -> void: _shape(block.pitch, v, block.fall))
	_choice_row(_group(row, tr("builder.roof.side.label")), choices.fall, int(block.fall),
		func(v: int) -> void: _shape(block.pitch, block.ridge, v))


## A group of the roof's row: its name over its controls.
func _group(row: HBoxContainer, title: String) -> VBoxContainer:
	var group := HudWidgets.vbox(row)
	HudWidgets.label(group, &"MutedLabel", title)
	return group


func _choice_row(group: VBoxContainer, options: Array, current: int, pick: Callable) -> void:
	var row := HudWidgets.choices(group, current, pick)
	for o: Dictionary in options:
		HudWidgets.choice(row, int(o.value), tr(o.label_key))


func _shape(pitch: float, ridge: int, fall: int) -> void:
	var r: Dictionary = CityStore.dispatch(&"body_roof_shape", [_picked.id, _picked.volume, pitch, ridge, fall])
	if not r.get("ok", false):
		tools.hint.emit(tools.active.refusal(r) if tools.active != null else tr(r.get("error", "tool.failed")), false)
	_stale = true
