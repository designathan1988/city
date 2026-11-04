## The game's shell: top bar (sky, clock, money, counts | speed pill | layers, simulation,
## camera, undo, redo, help, menu), the dock with the seven categories and their drawers, the hint bar, the inspector
## and the compass. Styles come from ui/theme/theme_builder.gd, texts from data/i18n/strings.csv, buttons from
## data/ui/drawers.json. The HUD only reads CityStore.snapshot and asks the ToolController for tools.
class_name RoadcraftHud
extends CanvasLayer

signal category_pressed(id: String)

const SKY := [["sun", "sky.day", "day"], ["moon", "sky.night", "night"], ["cycle", "sky.cycle", "cycle"]]
const SPEEDS := [["", 0], ["1×", 1], ["2×", 2], ["4×", 3]]

@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
@onready var rig: CityCamera = get_tree().get_first_node_in_group(&"camera_rig")
@onready var root: Control
@onready var drawer: HudDrawer
@onready var popover: HudPopover
@onready var inspector: InspectorPanel
@onready var actions: HudActions
@onready var _clock: Label
@onready var _money: Label
@onready var _stats: Label
@onready var _hint: Label
@onready var _speed_buttons: Array[Button] = []
signal sky_chosen(mode: String)

@onready var _sky_buttons: Array[Button] = []
@onready var _action_buttons: Dictionary = {}
@onready var _history_buttons: Dictionary = {}
@onready var _cat_buttons := {}
@onready var _needle: Control


func _ready() -> void:
	add_to_group(&"hud")
	root = Control.new()
	root.theme = ThemeBuilder.theme()
	root.mouse_filter = Control.MOUSE_FILTER_IGNORE
	root.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(root)
	_build_top_bar()
	_build_dock()
	_build_compass()
	_build_hint()
	drawer = HudDrawer.new()
	root.add_child(drawer)
	drawer.tool_requested.connect(_on_tool_requested)
	drawer.closed.connect(_on_drawer_closed)
	popover = HudPopover.new()
	root.add_child(popover)
	actions = HudActions.new()
	add_child(actions)
	popover.menu_action.connect(actions.menu)
	popover.open_slot.connect(actions.load_slot)
	inspector = InspectorPanel.new()
	root.add_child(inspector)
	CityStore.changed.connect(_on_changed)
	if tools:
		tools.hint.connect(show_hint)
		tools.inspected.connect(func(info: Dictionary) -> void: inspector.show_info(info, info.get("at", Vector3.ZERO)))
	_set_sky("day")


func _build_top_bar() -> void:
	var bar := HudWidgets.panel(root, &"Surface")
	bar.anchor_right = 1.0
	bar.offset_left = HudWidgets.px("margin")
	bar.offset_top = HudWidgets.px("margin")
	bar.offset_right = -HudWidgets.px("margin")
	bar.custom_minimum_size.y = HudWidgets.px("hud_h")
	var row := HudWidgets.hbox(bar, &"Gap")
	var left := HudWidgets.hbox(row, &"Gap")
	left.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	var sky := HudWidgets.hbox(HudWidgets.panel(left, &"Seg"))
	for s in SKY:
		var b := HudWidgets.button(&"SegButton", s[0], "", s[1], "seg_min_w", "seg_h")
		b.toggle_mode = true
		b.pressed.connect(_set_sky.bind(s[2]))
		sky.add_child(b)
		_sky_buttons.append(b)
	_clock = HudWidgets.label(left, &"ClockLabel")
	_money = HudWidgets.label(left, &"MoneyLabel")
	_money.tooltip_text = tr("ui.money.tip").format({"refund": int(round(DataTables.tuning("economy_refund_share") * Formatters.PERCENT))})
	_money.mouse_filter = Control.MOUSE_FILTER_PASS
	_stats = HudWidgets.label(left, &"StatsLabel")
	var pill := HudWidgets.hbox(HudWidgets.panel(row, &"Seg"))
	for s in SPEEDS:
		var b := HudWidgets.button(&"SegButton", "pause" if s[0] == "" else "", s[0], "sim.speed", "seg_min_w", "seg_h")
		b.toggle_mode = true
		b.pressed.connect(func() -> void: CityStore.dispatch(&"set_speed", [s[1]]))
		pill.add_child(b)
		_speed_buttons.append(b)
	var right := HudWidgets.hbox(row, &"Pad")
	right.alignment = BoxContainer.ALIGNMENT_END
	right.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	for a in DataTables.ui("drawers").actions:
		var b := HudWidgets.button(&"IconButton", a.icon, "", a.tip_key, "icon_btn", "icon_btn")
		right.add_child(b)
		if a.has("popover"):
			b.toggle_mode = true
			b.pressed.connect(open_popover.bind(a.popover))
			_action_buttons[a.popover] = b
		elif a.has("action"):
			b.pressed.connect(func() -> void: actions.history(a.action))
			_history_buttons[a.action] = b
		else:
			HudWidgets.soon(b, a.tip_key)


func _build_dock() -> void:
	var dock := HudWidgets.panel(root, &"Dock")
	dock.anchor_left = 0.5
	dock.anchor_right = 0.5
	dock.anchor_top = 1.0
	dock.anchor_bottom = 1.0
	dock.grow_horizontal = Control.GROW_DIRECTION_BOTH
	dock.grow_vertical = Control.GROW_DIRECTION_BEGIN
	dock.offset_bottom = -HudWidgets.px("margin")
	var row := HudWidgets.hbox(dock)
	for c in DataTables.ui("drawers").categories:
		var variation := &"DockDanger" if c.get("danger", false) else &"DockButton"
		var b := HudWidgets.button(variation, c.icon, tr(c.label_key), c.tip_key, "cat_w", "cat_h")
		b.toggle_mode = true
		b.icon_alignment = HORIZONTAL_ALIGNMENT_CENTER
		b.vertical_icon_alignment = VERTICAL_ALIGNMENT_TOP
		b.pressed.connect(open_category.bind(c.id))
		row.add_child(b)
		_cat_buttons[c.id] = b


## Compass: a round button bottom-right whose needle turns with the camera; a click puts north up.
func _build_compass() -> void:
	var c := HudWidgets.button(&"CompassButton", "", "", "camera.north", "compass", "compass")
	c.anchor_left = 1.0
	c.anchor_right = 1.0
	c.anchor_top = 1.0
	c.anchor_bottom = 1.0
	c.offset_left = -HudWidgets.px("compass") - HudWidgets.px("compass_margin")
	c.offset_top = -HudWidgets.px("compass") - HudWidgets.px("compass_margin")
	c.offset_right = -HudWidgets.px("compass_margin")
	c.offset_bottom = -HudWidgets.px("compass_margin")
	c.pressed.connect(func() -> void: if rig: rig.north())
	root.add_child(c)
	var needle := TextureRect.new()
	needle.texture = HudWidgets.icon("compass_full")
	needle.expand_mode = TextureRect.EXPAND_IGNORE_SIZE
	needle.stretch_mode = TextureRect.STRETCH_KEEP_ASPECT_CENTERED
	needle.mouse_filter = Control.MOUSE_FILTER_IGNORE
	needle.set_anchors_preset(Control.PRESET_FULL_RECT)
	needle.pivot_offset = Vector2.ONE * HudWidgets.px("compass") * 0.5
	c.add_child(needle)
	_needle = needle


## Hint bar above the drawer: the tool's instruction, the price while aiming, refusals.
func _build_hint() -> void:
	var p := HudWidgets.panel(root, &"Toast")
	p.anchor_left = 0.5
	p.anchor_right = 0.5
	p.grow_horizontal = Control.GROW_DIRECTION_BOTH
	p.offset_top = HudWidgets.px("toast_top")
	_hint = HudWidgets.label(p, &"BodyLabel")
	_hint.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	p.hide()


func show_hint(text: String, ok: bool) -> void:
	var p := _hint.get_parent() as PanelContainer
	p.theme_type_variation = &"Toast" if ok else &"DangerToast"
	_hint.theme_type_variation = &"BodyLabel" if ok else &"DangerLabel"
	_hint.text = text
	p.visible = text != ""
	p.reset_size()
	p.offset_left = -p.size.x * 0.5
	p.offset_right = p.size.x * 0.5


func open_category(id: String) -> void:
	var open: String = "" if drawer.category.get("id", "") == id else id
	for k in _cat_buttons:
		_cat_buttons[k].set_pressed_no_signal(k == open)
	category_pressed.emit(open)
	if open == "":
		drawer.close()
		return
	for c in DataTables.ui("drawers").categories:
		if c.id == open:
			drawer.open(c)


func open_popover(which: String) -> void:
	var b: Button = _action_buttons.get(which, _action_buttons.menu)
	var opened := popover.toggle(which, b.get_global_rect())
	for k in _action_buttons:
		_action_buttons[k].set_pressed_no_signal(opened and k == which)


func _on_tool_requested(tool: String, params: Dictionary) -> void:
	if tools:
		tools.set_tool(ToolController.Tool[tool], params)


func _on_drawer_closed() -> void:
	for k in _cat_buttons:
		_cat_buttons[k].set_pressed_no_signal(false)
	if tools:
		tools.set_tool(ToolController.Tool.NONE)
	show_hint("", true)


func _set_sky(mode: String) -> void:
	for i in SKY.size():
		_sky_buttons[i].set_pressed_no_signal(SKY[i][2] == mode)
	sky_chosen.emit(mode)


func _process(_delta: float) -> void:
	if _needle and rig:
		_needle.rotation = -rig.yaw


func _on_changed(s: Dictionary) -> void:
	if not s.get("ready", false):
		return
	var h: float = s.hour
	var mph: float = CityStore.constants.minutes_per_hour
	_clock.text = "%02d:%02d" % [int(h), int(fmod(h * mph, mph))]
	_money.text = Formatters.format_money(s.money)
	var parts := []
	if s.vehicles > 0:
		parts.append(tr("status.vehicles.other").format({"count": Formatters.format_int(s.vehicles)}))
	if s.people > 0:
		parts.append(tr("status.people.other").format({"count": Formatters.format_int(s.people)}))
	_stats.text = " · ".join(parts)
	_stats.tooltip_text = tr("ui.stats.tip").format({"residents": Formatters.format_int(s.residents),
		"jobs": Formatters.format_int(s.jobs), "buildings": Formatters.format_int(s.buildings), "speed": "—"})
	for i in SPEEDS.size():
		_speed_buttons[i].set_pressed_no_signal(SPEEDS[i][1] == int(s.speed))
	for action in _history_buttons:
		var b: Button = _history_buttons[action]
		b.disabled = not s.get("can_" + action, false)
		var label: String = s.get(action + "_label", "")
		b.tooltip_text = tr("action." + action) + ("" if label.is_empty() else " · " + tr(label))
