## Top-bar popovers: Layers, Simulation (ui/pop_bodies.gd), Camera (the buttons, the zoom and the centre), Help and Menu. Content comes
## from data/ui/drawers.json "popovers"; rows without an action the core supports are drawn "being built". One open at a time,
## under its button; a press outside or Escape closes it.
class_name HudPopover
extends PanelContainer

## Emitted by Menu rows: "new_empty", "new_city", "new_large", "save", "save_map", "load_last", "open", "graphics", "effects", "about".
signal menu_action(action: String)
## Emitted by a row of the Open popover: the save slot to load.
signal open_slot(slot: String)
signal closed

const METRES_PER_KM := 1000.0
const PERCENT := 100.0

@onready var rig: CityCamera = get_tree().get_first_node_in_group(&"camera_rig")
@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
## Value labels of the Simulation metrics and the Camera readouts, by name (rebuilt with the popover).
@onready var _metrics: Dictionary = {}
@onready var _since := 0.0
@onready var _under := Rect2()


func _ready() -> void:
	theme_type_variation = &"Popover"
	hide()
	CityStore.changed.connect(_on_changed)


## Opens `which` under the button rect (screen coordinates); the same one again closes it.
func toggle(which: String, under: Rect2) -> bool:
	if visible and get_meta(&"kind", "") == which:
		close()
		return false
	set_meta(&"kind", which)
	_under = under
	HudWidgets.clear(self)
	_metrics.clear()
	var spec: Dictionary = DataTables.ui("drawers").popovers[which]
	var col := HudWidgets.vbox(self, &"Pad")
	if spec.has("title_key"):
		HudWidgets.label(col, &"TitleLabel", tr(spec.title_key).to_upper())
	call("_build_" + which, col, spec)
	show()
	reset_size()
	var vw := get_viewport_rect().size.x
	position = Vector2(minf(under.end.x, vw - HudWidgets.px("margin")) - size.x, under.end.y + HudWidgets.px("pad"))
	_on_changed(CityStore.snapshot)
	return true


## The language changed: the open popover, drawn again where it stands.
func relabel() -> void:
	if visible:
		hide()
		toggle(get_meta(&"kind", ""), _under)


func close() -> bool:
	if not visible:
		return false
	hide()
	closed.emit()
	return true


## A press outside closes it; a press on its own button is that button's toggle.
func _input(event: InputEvent) -> void:
	var b := event as InputEventMouseButton
	if visible and b != null and b.pressed and not get_global_rect().has_point(b.position) and b.position.y > HudWidgets.px("toast_top"):
		close()


func _row(parent: Control, r: Dictionary) -> Button:
	var b := HudWidgets.button(&"Row", r.icon, tr(r.label_key), "", "pop_w", "row_h")
	b.alignment = HORIZONTAL_ALIGNMENT_LEFT
	parent.add_child(b)
	if not r.has("action"):
		HudWidgets.soon(b, r.label_key)
	return b


## Switch rows: what shows over the map, kept across sessions (Settings); the popover stays open.
func _build_layers(col: Control, spec: Dictionary) -> void:
	for r in spec.rows:
		var b := _row(col, r)
		var sw := HudSwitch.new()
		sw.on = Settings.layer_on(r.action) if r.has("action") else false
		b.add_child(sw)
		if r.has("action"):
			b.toggle_mode = true
			b.button_pressed = sw.on
			b.toggled.connect(func(v: bool) -> void:
				sw.on = v
				sw.queue_redraw()
				Settings.set_layer(r.action, v))


## The saved cities, newest first (CityStore.saves.list_saves); a click opens one.
func _build_files(col: Control, spec: Dictionary) -> void:
	var saves := CityStore.saves.list_saves()
	var other := HudWidgets.button(&"Row", "m_open", tr("ui.menu.openFile"), "", "pop_w", "row_h")
	other.alignment = HORIZONTAL_ALIGNMENT_LEFT
	other.pressed.connect(func() -> void:
		close()
		menu_action.emit("open_file"))
	col.add_child(other)
	if saves.is_empty():
		HudWidgets.label(col, &"MutedLabel", tr(spec.empty_key))
	for f in saves:
		var when := Time.get_datetime_string_from_unix_time(f.modified, true)
		var b := HudWidgets.button(&"Row", "m_open", "%s  ·  %s" % [f.slot, when], "", "pop_w", "row_h")
		b.alignment = HORIZONTAL_ALIGNMENT_LEFT
		b.pressed.connect(func() -> void:
			close()
			open_slot.emit(f.slot))
		col.add_child(b)


func _build_menu(col: Control, spec: Dictionary) -> void:
	var km: int = int(DataTables.catalog().maps.maps[0].tiles * DataTables.tuning("world_tile_m") / METRES_PER_KM)
	for r in spec.rows:
		if r.has("seg"):
			call("_seg_" + r.seg, col, r)
		elif r.has("rule"):
			col.add_child(HSeparator.new())
		else:
			var b := _row(col, r)
			b.text = b.text.format({"km": km})
			if r.has("action"):
				b.pressed.connect(func() -> void:
					close()
					menu_action.emit(r.action))


func _seg_quality(col: Control, r: Dictionary) -> void:
	var items := []
	for p: String in GraphicsPanel.PRESETS:
		items.append([p, tr("quality." + p)])
	var row := HudWidgets.option_row(col, tr(r.label_key))
	var box := HFlowContainer.new()
	box.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	row.add_child(box)
	HudWidgets.choices(box, items, Settings.preset, Settings.set_preset)


func _seg_paint(col: Control, r: Dictionary) -> void:
	var row := HudWidgets.option_row(col, tr(r.label_key))
	var items := []
	var styles: Array = DataTables.render().streets.markings.styles
	for i in styles.size():
		items.append([i, tr("paint." + styles[i].id)])
	HudWidgets.choices(row, items, int(CityStore.snapshot.get("marking_style", 0)), func(v: int) -> void:
		CityStore.dispatch(&"set_marking_style", [v, true]))


func _seg_language(col: Control, r: Dictionary) -> void:
	var row := HudWidgets.option_row(col, tr(r.label_key))
	HudWidgets.choices(row, StartScreen.LANGUAGES, TranslationServer.get_locale(), Settings.set_language)


func _build_camera(col: Control, spec: Dictionary) -> void:
	var row := HudWidgets.hbox(col)
	for c in spec.buttons:
		var b := HudWidgets.button(&"IconButton", c.icon, "", c.label_key, "icon_btn", "icon_btn")
		row.add_child(b)
		if c.has("action"):
			b.pressed.connect(_camera.bind(c.action))
			b.toggle_mode = c.action == "perspective"
			b.set_pressed_no_signal(b.toggle_mode and rig.camera.projection == Camera3D.PROJECTION_PERSPECTIVE)
		else:
			HudWidgets.soon(b, c.label_key)
	col.add_child(HSeparator.new())
	var grid := PopBodies.grid(col)
	for key: String in ["camera.zoom", "camera.centre"]:
		_metrics[key] = PopBodies.metric(grid, tr(key))
	_read_camera()


func _camera(action: String) -> void:
	match action:
		"turn_left": rig.turn(-1)
		"turn_right": rig.turn(1)
		"tilt_down": rig.tilt(-1)
		"tilt_up": rig.tilt(1)
		"north": rig.north()
		"reset": rig.reset_view()
		"perspective":
			var ortho := rig.camera.projection == Camera3D.PROJECTION_ORTHOGONAL
			rig.camera.projection = Camera3D.PROJECTION_PERSPECTIVE if ortho else Camera3D.PROJECTION_ORTHOGONAL


func _build_sim(col: Control, spec: Dictionary) -> void:
	PopBodies.sim(col, spec, _metrics)


func _build_help(col: Control, _spec: Dictionary) -> void:
	var tool_name: String = ToolController.Tool.keys()[tools.tool] if tools else "NONE"
	PopBodies.help(col, DataTables.ui("help"), tool_name)


func _process(delta: float) -> void:
	if not visible or not _metrics.has("camera.zoom"):
		return
	_since += delta
	if _since >= CityStore.SNAPSHOT_INTERVAL_S:
		_since = 0.0
		_read_camera()


func _read_camera() -> void:
	var start: float = DataTables.game("camera").start_view_half
	_metrics["camera.zoom"].text = "%d %%" % int(round(start / rig.half * PERCENT))
	_metrics["camera.centre"].text = "X %d · Y %d" % [int(rig.focus.x), int(rig.focus.z)]


func _on_changed(s: Dictionary) -> void:
	if not visible or _metrics.is_empty() or not s.get("ready", false) or _metrics.has("camera.zoom"):
		return
	PopBodies.sync_sim(_metrics, s)
