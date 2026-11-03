## Top-bar popovers (Roadcraft hud.ts §7, specs ui.md §7): Layers, Simulation (the core's live metrics), Camera
## (turn, tilt, north, top, frame), Help (keys) and Menu (new map, new city). Content comes from data/ui/drawers.json
## "popovers"; rows without an action the core supports are drawn "being built". One open at a time, under its button.
class_name HudPopover
extends PanelContainer

## Emitted by Menu rows: "new_empty", "new_city", "save", "save_map", "load_last" or "open".
signal menu_action(action: String)
## Emitted by a row of the Open popover: the save slot to load.
signal open_slot(slot: String)

const METRES_PER_KM := 1000.0

@onready var rig: CityCamera = get_tree().get_first_node_in_group(&"camera_rig")
## Value labels of the Simulation metrics, by snapshot key (rebuilt with the popover).
@onready var _metrics: Dictionary = {}


func _ready() -> void:
	theme_type_variation = &"Popover"
	hide()
	CityStore.changed.connect(_on_changed)


## Opens `which` under the button rect (screen coordinates); the same one again closes it.
func toggle(which: String, under: Rect2) -> bool:
	if visible and get_meta(&"kind", "") == which:
		hide()
		return false
	set_meta(&"kind", which)
	HudWidgets.clear(self)
	_metrics.clear()
	var spec: Dictionary = DataTables.ui("drawers").popovers[which]
	var col := HudWidgets.vbox(self)
	if spec.has("title_key"):
		HudWidgets.label(col, &"TitleLabel", tr(spec.title_key).to_upper())
	call("_build_" + which, col, spec)
	show()
	reset_size()
	var vw := get_viewport_rect().size.x
	position = Vector2(minf(under.end.x, vw - HudWidgets.px("margin")) - size.x, under.end.y + HudWidgets.px("pad"))
	_on_changed(CityStore.snapshot)
	return true


func _row(parent: Control, r: Dictionary) -> Button:
	var b := HudWidgets.button(&"Row", r.icon, tr(r.label_key), "", "pop_w", "row_h")
	b.alignment = HORIZONTAL_ALIGNMENT_LEFT
	parent.add_child(b)
	if not r.has("action"):
		HudWidgets.soon(b, r.label_key)
	return b


## Switch rows (Roadcraft hud.ts:508-522): the overlay the core draws for each; the popover stays open.
func _build_layers(col: Control, spec: Dictionary) -> void:
	var on: Dictionary = CityStore.query(&"overlays")
	for r in spec.rows:
		var b := _row(col, r)
		if r.has("action"):
			b.toggle_mode = true
			b.button_pressed = on.get(r.action, false)
			b.toggled.connect(func(v: bool) -> void: CityStore.dispatch(&"set_overlay", [r.action, v]))


## The saved cities, newest first (CityStore.list_saves); a click opens one.
func _build_files(col: Control, spec: Dictionary) -> void:
	var saves := CityStore.list_saves()
	if saves.is_empty():
		HudWidgets.label(col, &"MutedLabel", tr(spec.empty_key))
	for f in saves:
		var when := Time.get_datetime_string_from_unix_time(f.modified, true)
		var b := HudWidgets.button(&"Row", "m_open", "%s  ·  %s" % [f.slot, when], "", "pop_w", "row_h")
		b.alignment = HORIZONTAL_ALIGNMENT_LEFT
		b.pressed.connect(func() -> void:
			hide()
			open_slot.emit(f.slot))
		col.add_child(b)


func _build_menu(col: Control, spec: Dictionary) -> void:
	var km: int = int(DataTables.catalog().maps.maps[0].tiles * DataTables.tuning("world_tile_m") / METRES_PER_KM)
	for r in spec.rows:
		var b := _row(col, r)
		b.text = b.text.format({"km": km})
		if r.has("action"):
			b.pressed.connect(func() -> void:
				hide()
				menu_action.emit(r.action))


func _build_camera(col: Control, spec: Dictionary) -> void:
	var row := HudWidgets.hbox(col)
	for c in spec.buttons:
		var b := HudWidgets.button(&"IconButton", c.icon, "", c.label_key, "icon_btn", "icon_btn")
		row.add_child(b)
		if c.has("action"):
			b.pressed.connect(_camera.bind(c.action))
		else:
			HudWidgets.soon(b, c.label_key)


func _camera(action: String) -> void:
	match action:
		"turn_left": rig.turn(-1)
		"turn_right": rig.turn(1)
		"tilt_down": rig.tilt(-1)
		"tilt_up": rig.tilt(1)
		"north": rig.north()
		"top": rig.top_view()
		"reset": rig.reset_view()


func _build_sim(col: Control, spec: Dictionary) -> void:
	var grid := GridContainer.new()
	grid.columns = 2
	col.add_child(grid)
	for m in spec.metrics:
		var name := HudWidgets.label(grid, &"MutedLabel", tr(m[0]).format({"count": ""}).strip_edges().capitalize())
		name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		var value := HudWidgets.label(grid, &"BodyLabel")
		value.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT
		_metrics[m[1]] = value


func _build_help(col: Control, spec: Dictionary) -> void:
	for g in spec.groups:
		HudWidgets.label(col, &"TitleLabel", tr(g.title_key).to_upper())
		var grid := GridContainer.new()
		grid.columns = 2
		col.add_child(grid)
		for r in g.rows:
			HudWidgets.label(grid, &"BodyLabel", tr(r[0]))
			HudWidgets.label(grid, &"MutedLabel", tr(r[1]))


func _on_changed(s: Dictionary) -> void:
	if not visible or _metrics.is_empty() or not s.get("ready", false):
		return
	for key in _metrics:
		_metrics[key].text = Formatters.format_int(s.get(key, 0))
