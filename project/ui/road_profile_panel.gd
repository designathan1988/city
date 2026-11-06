## Personalizar… (Roadcraft ui/roads/profileEditor.ts): the road picked in the catalogue, element by element, docked over the drawer. What stops it from
## being built is told by name and marked on the element; "use" makes the profile the road tool draws with, "save" keeps it under Meus modelos.
class_name RoadProfilePanel
extends PanelContainer

signal used(index: int)
signal applied(index: int, street: bool) # the road the Inspector has picked
signal saved
signal closed

const WIDTH_STEP_M := 0.5
const SPEED_STEP := 10.0
const SPEED_LEAST := 10.0
const SPEED_MOST := 130.0

var _base := 0
var _base_id := ""
var _spec: Dictionary = {}
var _selected := 1
var _limits: Dictionary = {}
var _view: RoadSectionView
var _total: Label
var _problem: Label
var _props: HBoxContainer
var _name: LineEdit
var _use: Button
var _street: Button
var _on_road := false


func _init() -> void:
	theme_type_variation = &"SidePanel"


func _ready() -> void:
	_limits = CityStore.query(&"road_spec_limits")
	var col := HudWidgets.vbox(self, &"Pad")
	var head := HudWidgets.hbox(col)
	HudWidgets.label(head, &"CardLabel", tr("profileEditor.title"))
	_total = HudWidgets.label(head, &"MetaLabel")
	_total.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_name = LineEdit.new()
	_name.placeholder_text = tr("profile.templateName")
	_name.custom_minimum_size.x = HudWidgets.px("rp_name_w")
	head.add_child(_name)
	_action(head, "profileEditor.saveShort", _save)
	_use = _action(head, "profileEditor.newRoads", _apply)
	_street = _action(head, "inspector.toStreet", _apply_street)
	_street.hide()
	_action(head, "profileEditor.close", func() -> void: closed.emit())
	_view = RoadSectionView.new()
	col.add_child(_view)
	_view.picked.connect(func(i: int) -> void:
		_selected = i
		_refresh())
	_view.widened.connect(_set_width)
	_view.moved.connect(func(from: int, to: int) -> void:
		RoadSpecOps.moved(_spec, from, to)
		_selected = to
		_refresh())
	var palette := HudWidgets.hbox(col)
	HudWidgets.label(palette, &"MetaLabel", tr("profileEditor.add"))
	for item: Array in DataTables.ui("drawers").profile_palette:
		_action(palette, item[1], _add.bind(item[0]))
	_props = HudWidgets.hbox(col)
	_problem = HudWidgets.label(col, &"MetaLabel")


## The road to start from: a profile of the catalogue, or a saved road's elements.
func open(profile: Dictionary, spec: Dictionary) -> void:
	_base = profile.index
	_base_id = profile.id
	_spec = spec.duplicate(true)
	_selected = mini(1, _spec.elements.size() - 1)
	_refresh()


func _action(row: Control, key: String, act: Callable) -> Button:
	var b := HudWidgets.button(&"Ghost", "", tr(key), "", "", "tab_h")
	b.pressed.connect(act)
	row.add_child(b)
	return b


func _add(kind: String) -> void:
	var at := RoadSpecOps.insertion_index(_spec, kind, _selected)
	_spec.elements.insert(at, RoadSpecOps.new_element(kind, _spec, DataTables.ui("drawers").profile_defaults, _limits.band))
	_selected = at
	_refresh()


func _set_width(index: int, metres: float) -> void:
	var e: Dictionary = _spec.elements[index]
	if not _limits.has(e.kind):
		return
	var lo: Vector2 = _limits[e.kind]
	RoadSpecOps.with_width(_spec, index, clampf(snappedf(metres, WIDTH_STEP_M), lo.x, lo.y))
	_refresh()


func _refresh() -> void:
	var issues: Array = CityStore.query(&"road_spec_issues", [_base, _spec])
	var flagged := []
	var texts := PackedStringArray()
	for i: Dictionary in issues:
		flagged.append_array(i.elements)
		texts.push_back(tr("profile.problem." + String(i.problem)))
	_view.show_spec(_spec, _selected, flagged)
	_total.text = tr("profileEditor.totalWidth").format({"width": Formatters.format_measure(RoadSpecOps.total(_spec))})
	_problem.text = " · ".join(texts) if not issues.is_empty() else tr("profileEditor.ok")
	_use.disabled = not issues.is_empty()
	_fill_props()


func _button(row: Control, key: String, act: Callable) -> void:
	_action(row, key, func() -> void:
		act.call()
		_refresh())


## What the element picked can change, then the road's own.
func _fill_props() -> void:
	HudWidgets.clear(_props)
	if _selected >= 0 and _selected < _spec.elements.size():
		var e: Dictionary = _spec.elements[_selected]
		if _limits.has(e.kind):
			_button(_props, "profileEditor.narrower", _set_width.bind(_selected, e.width - WIDTH_STEP_M))
			_button(_props, "profileEditor.wider", _set_width.bind(_selected, e.width + WIDTH_STEP_M))
		if e.kind == "lane":
			_button(_props, "profileEditor.dirForward" if e.dir < 0 else "profileEditor.dirBackward", func() -> void: e.dir = -e.dir)
			_button(_props, "profileEditor.useBus" if not e.get("bus", false) else "profileEditor.useAll", func() -> void: e.bus = not e.get("bus", false))
			_button(_props, "profileEditor.lineSolid" if not e.get("solid", false) else "profileEditor.lineDashed", func() -> void: e.solid = not e.get("solid", false))
		if e.kind == "footway" or e.kind == "median":
			_button(_props, "profileEditor.flush", func() -> void: e.flush = not e.get("flush", false))
		_button(_props, "profileEditor.remove", func() -> void:
			_spec.elements.remove_at(_selected)
			_selected = mini(_selected, _spec.elements.size() - 1))
	_button(_props, "profileEditor.slower", func() -> void: _spec.speed = maxf(_spec.speed - SPEED_STEP, SPEED_LEAST))
	HudWidgets.label(_props, &"MetaLabel", tr("profileEditor.speedValue").format({"speed": int(_spec.speed)}))
	_button(_props, "profileEditor.faster", func() -> void: _spec.speed = minf(_spec.speed + SPEED_STEP, SPEED_MOST))


## Opened from a road of the city: applied to it or to its whole street, instead of the road tool's.
func on_road() -> void:
	_on_road = true
	_use.text = tr("profileEditor.apply")
	_street.show()


func _apply() -> void:
	var index: int = CityStore.query(&"road_spec_make", [_base, _spec])
	if index >= 0 and _on_road:
		applied.emit(index, false)
	elif index >= 0:
		used.emit(index)


func _apply_street() -> void:
	var index: int = CityStore.query(&"road_spec_make", [_base, _spec])
	if index >= 0:
		applied.emit(index, true)


func _save() -> void:
	if RoadModels.save(_name.text, _base_id, _spec):
		saved.emit()
