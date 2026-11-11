## The game's shell: the top bar, the dock with the seven categories and their drawers, the tool's options at the bottom left, the
## hint bar, the toast, the inspector, the compass, the popovers and the panels (F7, F9, Shift+F9). Styles come from
## ui/theme/theme_builder.gd, texts from data/i18n/strings*.csv, buttons from data/ui/drawers.json. The HUD only reads
## CityStore.snapshot and asks the ToolController for tools.
class_name RoadcraftHud
extends CanvasLayer

signal category_pressed(id: String)

const CATEGORY := &"category" # a dock button's category id
const MENU := "menu" # the popover whose button stands for those with no button of their own

@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
@onready var rig: CityCamera = get_tree().get_first_node_in_group(&"camera_rig")
@onready var world: CityWorld = get_tree().get_first_node_in_group(&"world")
@onready var root: Control
@onready var top_bar: HudTopBar
@onready var drawer: HudDrawer
@onready var popover: HudPopover
@onready var inspector: InspectorPanel
@onready var _road_editor: RoadProfilePanel
@onready var actions: HudActions
@onready var health: HealthPanel
@onready var profiler: ProfilerHud
@onready var effects: EffectsPanel
@onready var messages: HudMessages
@onready var _cat_buttons: Array[Button] = []
@onready var _dialog: HudDialog


func _ready() -> void:
	add_to_group(&"hud")
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
	root.add_child(JunctionMarks.new())
	root.add_child(PreviewTag.new())
	messages = HudMessages.new(root)
	add_child(messages)
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
	inspector.edit_profile.connect(_edit_road_profile.bind(root))
	var selection := SelectionOverlay.new(self)
	root.add_child(selection)
	root.move_child(selection, 0)
	health = HealthPanel.new()
	root.add_child(health)
	profiler = ProfilerHud.new()
	root.add_child(profiler)
	effects = EffectsPanel.new()
	root.add_child(effects)
	root.add_child(HudTip.new())
	add_child(GameKeys.new(self))
	if tools:
		tools.hint.connect(show_hint)
		tools.inspected.connect(_on_inspector)
		tools.tool_changed.connect(func(t: ToolController.Tool, _p: Dictionary) -> void: Settings.set_zone_tool(t == ToolController.Tool.ZONE))
		tools.set_tool(ToolController.Tool.INSPECT) # with nothing in hand a click asks what is there, as in the reference


## "Editar perfil" of the Inspector: the same editor as the road tool's, over the dock, working on that road.
func _edit_road_profile(profile: Dictionary, spec: Dictionary, at: Vector3, root: Control) -> void:
	if is_instance_valid(_road_editor):
		_road_editor.queue_free()
	_road_editor = RoadProfilePanel.new()
	root.add_child(_road_editor)
	_road_editor.set_anchors_and_offsets_preset(Control.PRESET_CENTER_BOTTOM, Control.PRESET_MODE_MINSIZE)
	_road_editor.grow_horizontal = Control.GROW_DIRECTION_BOTH
	_road_editor.grow_vertical = Control.GROW_DIRECTION_BEGIN
	_road_editor.offset_bottom = -HudWidgets.px("dock_h") - 2.0 * HudWidgets.px("margin")
	_road_editor.closed.connect(_road_editor.queue_free)
	_road_editor.applied.connect(func(index: int, street: bool) -> void:
		CityStore.dispatch(&"upgrade_road", [at, index, street])
		_road_editor.queue_free()
		inspector.refresh())
	_road_editor.on_road()
	_road_editor.open(profile, spec)


func _on_inspector(info: Dictionary) -> void:
	inspector.show_info(info, info.get("at", Vector3.ZERO))


## The language changed: every text of the shell in the new one; what was open stays open.
func _notification(what: int) -> void:
	if what != NOTIFICATION_TRANSLATION_CHANGED or root == null or not is_node_ready():
		return
	for c: Dictionary in DataTables.ui("drawers").categories:
		var b := _category_button(c.id)
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
		b.set_meta(CATEGORY, c.id)
		row.add_child(b)
		_cat_buttons.append(b)


func _category_button(id: String) -> Button:
	for b in _cat_buttons:
		if b.get_meta(CATEGORY) == id:
			return b
	return null


## The tool's hint: the price while aiming and refusals.
func show_hint(text: String, ok: bool) -> void:
	messages.hint(text, ok)


## The game's answers (saved, refused, undone): a toast that goes by itself, apart from the tool's hint.
func flash(text: String, ok: bool) -> void:
	messages.flash(text, ok)


func open_category(id: String) -> void:
	var open: String = "" if drawer.category.get("id", "") == id else id
	for b in _cat_buttons:
		b.set_pressed_no_signal(b.get_meta(CATEGORY) == open)
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


## A popover under its button (one with no button of its own under the menu's), that button pressed while it is open.
func open_popover(which: String) -> void:
	var b := top_bar.popover_button(which)
	if b == null:
		b = top_bar.popover_button(MENU)
	var opened := popover.toggle(which, b.get_global_rect())
	top_bar.press_popover(which if opened else "")


func _on_popover_closed() -> void:
	top_bar.press_popover("")


## A modal dialog over the game: graphics, about.
func open_dialog(d: HudDialog) -> void:
	if is_instance_valid(_dialog):
		_dialog.close()
	_dialog = d
	root.add_child(d)


func toggle_effects() -> void:
	effects.toggle()


## Escape, one step back (Roadcraft main.ts escape): the open panel, what is being drawn, the inspector, then the tool in
## hand; false: nothing to put down.
func escape() -> bool:
	if popover.close() or health.close() or effects.close():
		return true
	if rig and rig.gestures.active():
		rig.gestures.cancel()
		return true
	if tools and tools.active and tools.active.in_progress():
		tools.active.cancel()
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
	for b in _cat_buttons:
		b.set_pressed_no_signal(false)
	if tools:
		tools.set_tool(ToolController.Tool.INSPECT)
	show_hint("", true)
