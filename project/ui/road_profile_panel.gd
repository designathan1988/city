## Customise… (Roadcraft ui/roads/profileEditor.ts): the road picked in the catalogue, element by element, docked over the drawer. What stops it from
## being built is told by name and marked on the element; "use" makes the profile the road tool draws with, "save" keeps it under My roads.
class_name RoadProfilePanel
extends PanelContainer

signal used(index: int)
signal applied(index: int, street: bool) # the road the Inspector has picked
signal saved
signal closed

const WIDEN_BY_M := 0.5
const PACE_BY := 10.0
const SPEED_LEAST := 10.0
const SPEED_MOST := 130.0
## A footway's or a median's paving, by the core's number of it (profile_types.h Paving), and the centre lines the editor offers.
const PAVING_KEYS := ["profile.materialName.concrete", "profile.materialName.pavers", "profile.materialName.stone", "profile.materialName.grass"]
const AXIS_LINES := ["auto", "dashed", "solid", "double"]

@export var edit := RoadSpecEdit.new()
@onready var _view: RoadSectionView
@onready var _total: Label
@onready var _problem: Label
@onready var _props: HBoxContainer
@onready var _road: VBoxContainer
@onready var _name: LineEdit
@onready var _use: Button
@onready var _keep: Button
@onready var _street: Button


func _init() -> void:
	theme_type_variation = &"SidePanel"


func _ready() -> void:
	edit.limits = CityStore.query(&"road_spec_limits")
	var col := HudWidgets.vbox(self, &"Pad")
	var head := HudWidgets.hbox(col)
	HudWidgets.label(head, &"CardLabel", tr("profileEditor.title"))
	_total = HudWidgets.label(head, &"MetaLabel")
	_total.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_name = LineEdit.new()
	_name.placeholder_text = tr("profile.templateName")
	_name.custom_minimum_size.x = HudWidgets.px("rp_name_w")
	head.add_child(_name)
	_keep = _action(head, "profileEditor.saveShort", _save)
	_use = _action(head, "profileEditor.newRoads", _apply)
	_street = _action(head, "inspector.toStreet", _apply_street)
	_street.hide()
	_action(head, "profileEditor.close", func() -> void: closed.emit())
	_view = RoadSectionView.new()
	col.add_child(_view)
	_view.picked.connect(func(i: int) -> void:
		edit.selected = i
		_refresh())
	_view.widened.connect(_set_width)
	_view.moved.connect(func(from: int, to: int) -> void:
		RoadSpecOps.moved(edit.spec, from, to)
		edit.selected = to
		_refresh())
	var palette := HudWidgets.hbox(col)
	HudWidgets.label(palette, &"MetaLabel", tr("profileEditor.add"))
	for item: Array in DataTables.ui("drawers").profile_palette:
		_action(palette, item[1], _add.bind(item[0]))
	_props = HudWidgets.hbox(col)
	_road = HudWidgets.vbox(col)
	_problem = HudWidgets.label(col, &"MetaLabel")


## The road to start from: a profile of the catalogue, or a saved road's elements.
func open(profile: Dictionary, spec: Dictionary) -> void:
	edit.base = profile.index
	edit.base_id = profile.id
	edit.spec = spec.duplicate(true)
	edit.selected = mini(1, edit.spec.elements.size() - 1)
	_fill_class()
	_refresh()


## The class the road is made in (its carriageway, its rank and what it may carry come with it): any of the catalogue's.
func _fill_class() -> void:
	HudWidgets.clear(_road)
	var picks := []
	for p: Dictionary in CityStore.cached(&"road_profiles"):
		picks.append([p.index, tr(p.name_key)])
	HudWidgets.dropdown(_road, tr("profileEditor.class"), picks, edit.base, _set_class)


func _set_class(index: int) -> void:
	for p: Dictionary in CityStore.cached(&"road_profiles"):
		if p.index == index:
			edit.base = index
			edit.base_id = p.id
	_refresh()


func _action(row: Control, key: String, act: Callable) -> Button:
	var b := HudWidgets.button(&"Ghost", "", tr(key), "", "", "tab_h")
	b.pressed.connect(act)
	row.add_child(b)
	return b


func _add(kind: String) -> void:
	var at := RoadSpecOps.insertion_index(edit.spec, kind, edit.selected)
	edit.spec.elements.insert(at, RoadSpecOps.new_element(kind, edit.spec, DataTables.ui("drawers").profile_defaults, edit.limits.band))
	edit.selected = at
	_refresh()


func _set_width(index: int, metres: float) -> void:
	var e: Dictionary = edit.spec.elements[index]
	if not edit.limits.has(e.kind):
		return
	var lo: Vector2 = edit.limits[e.kind]
	if RoadSpecOps.with_width(edit.spec, index, clampf(snappedf(metres, WIDEN_BY_M), lo.x, lo.y)):
		_refresh() # a drag that snaps to the width it had changes nothing, and nothing is drawn again


func _refresh() -> void:
	var issues: Array = CityStore.query(&"road_spec_issues", [edit.base, edit.spec])
	var flagged := []
	var texts := PackedStringArray()
	for i: Dictionary in issues:
		flagged.append_array(i.elements)
		texts.push_back(tr("profile.problem." + String(i.problem)))
	_view.show_spec(edit.spec, edit.selected, flagged)
	_total.text = tr("profileEditor.totalWidth").format({"width": Formatters.format_measure(RoadSpecOps.total(edit.spec))})
	_problem.text = " · ".join(texts) if not issues.is_empty() else tr("profileEditor.ok")
	_use.disabled = not issues.is_empty()
	_keep.disabled = not issues.is_empty() # a road with a problem is neither used nor kept
	_fill_props()


func _button(row: Control, key: String, act: Callable) -> void:
	_action(row, key, func() -> void:
		act.call()
		_refresh())


## A button that names a choice and its value ("Paving: Stone"); a press takes the next one.
func _choice(row: Control, name_key: String, value_key: String, act: Callable) -> void:
	var b := _action(row, name_key, func() -> void:
		act.call()
		_refresh())
	b.text = "%s: %s" % [tr(name_key), tr(value_key)]


## What the element picked can change, then the road's own.
func _fill_props() -> void:
	HudWidgets.clear(_props)
	if edit.selected >= 0 and edit.selected < edit.spec.elements.size():
		var e: Dictionary = edit.spec.elements[edit.selected]
		if edit.limits.has(e.kind):
			_action(_props, "profileEditor.narrower", _set_width.bind(edit.selected, e.width - WIDEN_BY_M))
			_action(_props, "profileEditor.wider", _set_width.bind(edit.selected, e.width + WIDEN_BY_M))
		if e.kind == "lane":
			_button(_props, "profileEditor.dirForward" if e.dir < 0 else "profileEditor.dirBackward", func() -> void: e.dir = -e.dir)
			_button(_props, "profileEditor.useBus" if not e.get("bus", false) else "profileEditor.useAll", func() -> void: e.bus = not e.get("bus", false))
			_button(_props, "profileEditor.lineSolid" if not e.get("solid", false) else "profileEditor.lineDashed", func() -> void: e.solid = not e.get("solid", false))
		if e.kind == "footway" or e.kind == "median":
			_button(_props, "profileEditor.flush", func() -> void: e.flush = not e.get("flush", false))
			var pave: int = clampi(int(e.get("pave", 0)), 0, PAVING_KEYS.size() - 1)
			_choice(_props, "profileEditor.paving", PAVING_KEYS[pave], func() -> void: e.pave = (pave + 1) % PAVING_KEYS.size())
		_button(_props, "profileEditor.remove", func() -> void:
			edit.spec.elements.remove_at(edit.selected)
			edit.selected = mini(edit.selected, edit.spec.elements.size() - 1))
	_button(_props, "profileEditor.slower", func() -> void: edit.spec.speed = maxf(edit.spec.speed - PACE_BY, SPEED_LEAST))
	HudWidgets.label(_props, &"MetaLabel", tr("profileEditor.speedValue").format({"speed": int(edit.spec.speed)}))
	_button(_props, "profileEditor.faster", func() -> void: edit.spec.speed = minf(edit.spec.speed + PACE_BY, SPEED_MOST))
	# the line between the two directions: the road's own rule, or the one chosen (a road that paints none shows as automatic)
	var line: int = maxi(0, AXIS_LINES.find(String(edit.spec.get("centre", "auto"))))
	_choice(_props, "profileEditor.centreLine", "profileEditor.centreLineName." + AXIS_LINES[line],
		func() -> void: edit.spec.centre = AXIS_LINES[(line + 1) % AXIS_LINES.size()])


## Opened from a road of the city: applied to it or to its whole street, instead of the road tool's.
func on_road() -> void:
	edit.on_road = true
	_use.text = tr("profileEditor.apply")
	_street.show()


## The profile composed made a road by the core; refused (told by CityStore.command_failed), the editor stays open on it.
func _apply() -> void:
	var made: Dictionary = CityStore.dispatch(&"road_spec_make", [edit.base, edit.spec])
	if not made.get("ok", false):
		_problem.text = tr(String(made.get("error", ""))) # why it was not made: a problem, or no room for another road of the player's
		return
	if edit.on_road:
		applied.emit(int(made.index), false)
	else:
		used.emit(int(made.index))


func _apply_street() -> void:
	var made: Dictionary = CityStore.dispatch(&"road_spec_make", [edit.base, edit.spec])
	if made.get("ok", false):
		applied.emit(int(made.index), true)
	else:
		_problem.text = tr(String(made.get("error", "")))


## Kept under My roads: never a road with a problem (the button is off while one stands).
func _save() -> void:
	if not _keep.disabled and RoadModels.save(_name.text, edit.base_id, edit.spec):
		saved.emit()
