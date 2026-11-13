## The drawer over the dock: the open category's tabs and a close
## button in the head, its strip below (road catalogue, zone cards or the tool's note). A tab or card asks for a tool
## with `tool_requested`; tabs whose tool the core does not have yet are drawn "being built" (ui.soon). Cards and
## sliders of the Relief, Draw and Roundabout strips change the tool's parameters (ToolController remembers them).
class_name HudDrawer
extends PanelContainer

const FITS := &"fits_drawer" # what scrolls inside the room the drawer has: fit(width)

signal tool_requested(tool: String, params: Dictionary)
signal closed

const DENSITY_FIRST := 0

## The panel at the bottom left that holds the tool's options (the HUD makes it).
@export var options_panel: PanelContainer
@onready var options: VBoxContainer
## The road tool's options: its rows stand in the options panel while the road tool is in hand.
@onready var road_options := RoadOptions.new()
@onready var category: Dictionary = {}
@onready var catalog: RoadCatalog
@onready var _editor: RoadProfilePanel
@onready var _head: HBoxContainer
@onready var strip: Container
@onready var _tabs: Dictionary = {}
@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
@onready var _profile_ids: Dictionary = {}
@onready var _tab: Dictionary = {}
@onready var density := DENSITY_FIRST


func _ready() -> void:
	theme_type_variation = &"Surface"
	anchor_left = 0.5
	anchor_right = 0.5
	anchor_top = 1.0
	anchor_bottom = 1.0
	grow_vertical = Control.GROW_DIRECTION_BEGIN
	offset_bottom = -(HudWidgets.px("margin") + HudWidgets.px("dock_h") + HudWidgets.px("drawer_gap"))
	options_panel.custom_minimum_size.x = HudWidgets.px("opts_w")
	options_panel.anchor_top = 1.0
	options_panel.anchor_bottom = 1.0
	options_panel.grow_vertical = Control.GROW_DIRECTION_BEGIN
	options_panel.offset_left = HudWidgets.px("margin")
	options_panel.offset_right = HudWidgets.px("margin") + HudWidgets.px("opts_w")
	options_panel.offset_bottom = -HudWidgets.px("margin")
	options = HudWidgets.vbox(options_panel, &"Pad")
	options_panel.hide()
	var col := HudWidgets.vbox(self)
	_head = HudWidgets.hbox(col)
	strip = HudWidgets.hbox(col, &"Pad")
	for p in CityStore.cached(&"road_profiles"):
		_profile_ids[p.id] = p.index
	catalog = RoadCatalog.new()
	catalog.minimum_size_changed.connect(DrawerStrips.fit.bind(self))
	get_viewport().size_changed.connect(DrawerStrips.fit.bind(self))
	road_options.alignment_picked.connect(func(a: int) -> void: retool(a, "ROAD", "alignment"))
	road_options.option_picked.connect(func(key: String, value: Variant) -> void: retool(value, "ROAD", key))
	road_options.road_picked.connect(pick_road)
	catalog.profile_picked.connect(func(i: int) -> void:
		var upgrading := tools != null and tools.tool == ToolController.Tool.UPGRADE
		retool(i, "UPGRADE" if upgrading else "ROAD", "profile"))
	catalog.customise.connect(_customise.bind(col))
	hide()


## Customise…: the editor docked over the strip, one at a time.
func _customise(profile: Dictionary, spec: Dictionary, col: Control) -> void:
	if not is_instance_valid(_editor):
		_editor = RoadProfilePanel.new()
		col.add_child(_editor)
		col.move_child(_editor, 0)
		_editor.closed.connect(_editor.queue_free)
		_editor.used.connect(catalog.pick)
		_editor.saved.connect(catalog.saved_one)
	_editor.open(profile, spec)


## The catalogue and the road options leave the tree while another strip shows (_build_strip); a node out of the tree is not
## freed with it, so the drawer frees them then (else they outlived the game: orphan nodes at exit).
func _notification(what: int) -> void:
	if what == NOTIFICATION_PREDELETE:
		for kept: Control in [catalog, road_options]:
			if is_instance_valid(kept) and kept.get_parent() == null:
				kept.free()


func open(cat: Dictionary) -> void:
	category = cat
	if cat.get("no_drawer", false):
		hide()
	else:
		_build_head()
		show()
	var first: Dictionary = cat.tabs.filter(func(t: Dictionary) -> bool: return t.has("tool")).front()
	select_tab(first)


func close() -> void:
	hide()
	options_panel.hide()
	category = {}
	closed.emit()


func select_tab(tab: Dictionary) -> void:
	_tab = tab
	for id in _tabs:
		_tabs[id].set_pressed_no_signal(id == tab.id)
	_build_strip(tab)
	var params := {}
	if tab.has("profile"):
		params.profile = _profile_ids.get(tab.profile, 0)
	elif tab.tool == "ROAD" or tab.tool == "UPGRADE":
		params = tools.remembered.get(ToolController.Tool[tab.tool], {}).duplicate() if tools else {}
		params.profile = catalog.picked()
	elif tab.tool == "ZONE":
		params.zone = zone()
	elif tab.tool == "TRANSIT":
		params = { "kind": tab.kind, "mode": tab.mode }
	elif tools and ToolController.Tool.has(tab.tool):
		params = tools.remembered.get(ToolController.Tool[tab.tool], {}).duplicate()
	if tab.get("id", "") == "street" and params.is_empty():
		params = StreetStrip.first_piece(DataTables.ui("drawers").street)
	tool_requested.emit(tab.tool, params)
	if tab.get("id", "") == "street":
		StreetStrip.show(strip, tools, tool_requested)


## The tool in hand again with one parameter changed (a card or a slider of the strip).
func retool(value: Variant, tool: String, key: String) -> void:
	var params: Dictionary = tools.remembered.get(ToolController.Tool[tool], {}).duplicate() if tools else {}
	params[key] = value
	tool_requested.emit(tool, params)
	if key not in ["radius", "strength", "lanes", "width", "parking"]:
		DrawerStrips.light(strip, value)


## A road of the catalogue by id, as if its card were clicked.
func pick_road(id: String) -> void:
	catalog.pick(_profile_ids.get(id, 0))


func param(tool: String, key: String, fallback: Variant) -> Variant:
	return tools.last_param(ToolController.Tool[tool], key, fallback) if tools else fallback


## The zone the brush last painted (ToolController remembers it), else the first card's.
func zone() -> int:
	var first: int = category.cards[0].zone if category.has("cards") else 0
	return tools.last_param(ToolController.Tool.ZONE, "zone", first) if tools else first


## The language changed: what the drawer shows, in the new one.
func relabel() -> void:
	catalog.relabel()
	if not category.is_empty() and visible:
		_build_head()
		_build_strip(_tab)


func pick_zone(use: int) -> void:
	ZoneStrip.pick(self, use)


func _build_head() -> void:
	HudWidgets.clear(_head)
	_tabs.clear()
	for t in category.tabs:
		var b := HudWidgets.button(&"DrawerTab", t.icon, tr(t.label_key), "", "", "tab_h")
		b.toggle_mode = true
		if t.has("tool"):
			b.pressed.connect(select_tab.bind(t))
		else:
			HudWidgets.soon(b, t.label_key)
		_head.add_child(b)
		_tabs[t.id] = b
	var spacer := Control.new()
	spacer.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_head.add_child(spacer)
	var x := HudWidgets.button(&"IconButton", "close", "", "v2.close", "icon_btn", "tab_h")
	x.pressed.connect(close)
	_head.add_child(x)


func _build_strip(tab: Dictionary) -> void:
	for kept: Control in [catalog, road_options]:
		if kept.get_parent():
			kept.get_parent().remove_child(kept)
	HudWidgets.clear(strip)
	HudWidgets.clear(options)
	var kind: String = category.get("strip", "")
	if tab.get("tool", "") == "ROAD":
		options.add_child(road_options)
		road_options.refresh()
	if kind == "road_catalog" and (tab.tool == "ROAD" or tab.tool == "UPGRADE"):
		strip.add_child(catalog)
	elif kind == "zone_cards" and tab.tool == "ZONE":
		ZoneStrip.build(self)
	elif tab.get("tool", "") == "LOTS":
		LotStrip.build(self)
	elif tab.get("tool", "") == "ROUNDABOUT":
		var ring: Dictionary = category.ring
		HudWidgets.range_row(options, tr(ring.label_key), DataTables.tuning("roundabout_radius_min_m"),
			DataTables.tuning("roundabout_radius_max_m"), ring.step, param("ROUNDABOUT", "radius", ring.default), Formatters.format_length,
			retool.bind("ROUNDABOUT", "radius"))
	elif tab.get("tool", "") == "STRIKE":
		StrikeStrip.build(self)
	elif kind == "land_panels":
		LandStrips.build(self, strip, tab.get("tool", ""))
	elif tab.get("tool", "") == "BUILD":
		BuilderStrip.fill(strip, tab.id, tools)
	elif tab.get("kind", "") == "line":
		TransitStrip.build(strip)
	elif kind == "build_cards" and tab.get("tool", "") == "LOT":
		BuilderStrip.fill(strip, tab.id, tools)
		for c in category.cards:
			strip.add_child(DrawerStrips.card(c, "fn", param("LOT", "fn", 0), retool.bind("LOT", "fn")))
	_note(tab)
	settle()


## The tab in hand, built again: an option changed what it shows.
func rebuild() -> void:
	for t in category.tabs:
		if _tabs[t.id].button_pressed:
			_build_strip(t)


func _note(tab: Dictionary) -> void:
	var note := HudWidgets.label(strip, &"MutedLabel", tr(tab.get("note_key", "")) if tab.has("note_key") else "")
	note.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	note.custom_minimum_size.x = HudWidgets.px("note_w")
	note.visible = note.text != "" and strip.get_child_count() == 1 # a mode with nothing to pick says what it does


## The panels again after their content changed.
func settle() -> void:
	options_panel.visible = options.get_child_count() > 0
	options_panel.reset_size()
	options_panel.offset_top = -options_panel.size.y - HudWidgets.px("margin")
	DrawerStrips.fit.call_deferred(self)
