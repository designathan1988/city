## The game's shell: the top bar, the dock with the seven categories and their drawers, the tool's options at the bottom left, the
## hint bar, the toast, the inspector, the compass, the popovers and the panels (F7, F9, Shift+F9). Styles come from
## ui/theme/theme_builder.gd, texts from data/i18n/strings*.csv, buttons from data/ui/drawers.json. The HUD only reads
## CityStore.snapshot and asks the ToolController for tools.
class_name RoadcraftHud
extends CanvasLayer

signal category_pressed(id: String)

@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
@onready var rig: CityCamera = get_tree().get_first_node_in_group(&"camera_rig")
@onready var world: CityWorld = get_tree().get_first_node_in_group(&"world")
@onready var root: Control
@onready var top_bar: HudTopBar
@onready var drawer: HudDrawer
@onready var popover: HudPopover
@onready var inspector: InspectorPanel
@onready var actions: HudActions
@onready var health: HealthPanel
@onready var profiler: ProfilerHud
@onready var effects: EffectsPanel
@onready var _hint: Label
@onready var _cat_buttons := {}
@onready var _dialog: HudDialog
@onready var _toast_left := 0.0
@onready var _toast: PanelContainer
@onready var _toast_label: Label


func _ready() -> void:
	root = Control.new()
	root.theme = ThemeBuilder.theme()
	root.mouse_filter = Control.MOUSE_FILTER_IGNORE
	root.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(root)
	top_bar = HudTopBar.new()
	top_bar.world = world
	root.add_child(top_bar)
	_build_dock()
	root.add_child(HudCompass.new(rig))
	_build_hint()
	_build_toast()
	var options := HudWidgets.panel(root, &"Surface")
	drawer = HudDrawer.new()
	drawer.options_panel = options
	root.add_child(drawer)
	drawer.tool_requested.connect(_on_tool_requested)
	drawer.closed.connect(_on_drawer_closed)
	popover = HudPopover.new()
	root.add_child(popover)
	actions = HudActions.new()
	add_child(actions)
	popover.menu_action.connect(actions.menu)
	popover.open_slot.connect(actions.load_slot)
	popover.closed.connect(_on_popover_closed)
	top_bar.popover_pressed.connect(open_popover)
	top_bar.history_pressed.connect(actions.history)
	top_bar.impossible_pressed.connect(actions.show_impossible)
	inspector = InspectorPanel.new()
	root.add_child(inspector)
	health = HealthPanel.new()
	root.add_child(health)
	profiler = ProfilerHud.new()
	root.add_child(profiler)
	effects = EffectsPanel.new()
	root.add_child(effects)
	add_child(GameKeys.new(self))
	if tools:
		tools.hint.connect(show_hint)
		tools.inspected.connect(_on_inspector)
		tools.set_tool(ToolController.Tool.INSPECT) # with nothing in hand a click asks what is there, as in the reference
	top_bar.set_sky(world.sky_mode if world else "day")


func _on_inspector(info: Dictionary) -> void:
	inspector.show_info(info, info.get("at", Vector3.ZERO))


## The language changed: every text of the shell in the new one; what was open stays open.
func _notification(what: int) -> void:
	if what != NOTIFICATION_TRANSLATION_CHANGED or root == null or not is_node_ready():
		return
	for c: Dictionary in DataTables.ui("drawers").categories:
		var b: Button = _cat_buttons[c.id]
		b.text = tr(c.label_key)
		HudWidgets.tip_for(b, tr(c.tip_key))
	top_bar.relabel()
	drawer.relabel()
	popover.relabel()
	health.relabel()
	effects.relabel()


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


## Hint bar under the toast's place: the price while aiming and refusals (what a tool does is its drawer's note, as in the reference).
func _build_hint() -> void:
	var p := HudWidgets.panel(root, &"Toast")
	p.anchor_left = 0.5
	p.anchor_right = 0.5
	p.grow_horizontal = Control.GROW_DIRECTION_BOTH
	p.offset_top = HudWidgets.px("toast_top") + HudWidgets.px("hud_h")
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


func _build_toast() -> void:
	_toast = HudWidgets.panel(root, &"Toast")
	_toast.anchor_left = 0.5
	_toast.anchor_right = 0.5
	_toast.grow_horizontal = Control.GROW_DIRECTION_BOTH
	_toast.offset_top = HudWidgets.px("toast_top")
	_toast_label = HudWidgets.label(_toast, &"BodyLabel")
	_toast.hide()


## The game's answers (saved, refused, undone): a toast that goes by itself, apart from the tool's hint.
func flash(text: String, ok: bool) -> void:
	_toast.theme_type_variation = &"Toast" if ok else &"DangerToast"
	_toast_label.text = text
	_toast.show()
	_toast.reset_size()
	_toast.offset_left = -_toast.size.x * 0.5
	_toast.offset_right = _toast.size.x * 0.5
	_toast_left = HudWidgets.px("toast_s")


func _process(delta: float) -> void:
	if _toast_left > 0.0:
		_toast_left -= delta
		_toast.visible = _toast_left > 0.0


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


## A tab of a category, the category opened if it is not (the tool's letters).
func open_tab(cat: String, tab_id: String) -> void:
	if drawer.category.get("id", "") != cat:
		open_category(cat)
	for t: Dictionary in drawer.category.tabs:
		if t.id == tab_id and t.has("tool"):
			drawer.select_tab(t)


## The number row: the catalogue's road, the road tool in hand.
func pick_road(index: int) -> void:
	open_tab("roads", "draw")
	drawer.catalog.pick_index(index)


func open_popover(which: String) -> void:
	var buttons := top_bar.action_buttons
	var b: Button = buttons.get(which, buttons.menu)
	var opened := popover.toggle(which, b.get_global_rect())
	for k in buttons:
		buttons[k].set_pressed_no_signal(opened and k == which)


func _on_popover_closed() -> void:
	for k in top_bar.action_buttons:
		top_bar.action_buttons[k].set_pressed_no_signal(false)


## A modal dialog over the game: graphics, about.
func open_dialog(d: HudDialog) -> void:
	if is_instance_valid(_dialog):
		_dialog.close()
	_dialog = d
	root.add_child(d)


func toggle_effects() -> void:
	effects.toggle()


## Escape, one step back: the open panel, then the tool in hand; false: nothing to put down.
func escape() -> bool:
	if popover.close() or health.close() or effects.close():
		return true
	if rig and rig.gestures.active():
		rig.gestures.cancel()
		return true
	if inspector.visible:
		inspector.close()
		return true
	if drawer.category.is_empty():
		return false
	drawer.close()
	return true


func _on_tool_requested(tool: String, params: Dictionary) -> void:
	if tools:
		tools.set_tool(ToolController.Tool[tool], params)


func _on_drawer_closed() -> void:
	for k in _cat_buttons:
		_cat_buttons[k].set_pressed_no_signal(false)
	if tools:
		tools.set_tool(ToolController.Tool.INSPECT)
	show_hint("", true)
