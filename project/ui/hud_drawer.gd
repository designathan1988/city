## The drawer over the dock: the open category's tabs and a close
## button in the head, its strip below (road catalogue, zone cards or the tool's note). A tab or card asks for a tool
## with `tool_requested`; tabs whose tool the core does not have yet are drawn "being built" (ui.soon). Cards and
## sliders of the Relief, Draw and Roundabout strips change the tool's parameters (ToolController remembers them).
class_name HudDrawer
extends PanelContainer

const PERCENT := 100.0

signal tool_requested(tool: String, params: Dictionary)
signal closed

@onready var category: Dictionary = {}
@onready var catalog: RoadCatalog
## The road tool's options, a panel of its own beside the drawer (the HUD makes it).
@onready var options: RoadOptions
@onready var _head: HBoxContainer
@onready var _strip: Container
@onready var _tabs: Dictionary = {}
@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
@onready var _profile_ids: Dictionary = {}


func _ready() -> void:
	theme_type_variation = &"Surface"
	anchor_left = 0.5
	anchor_right = 0.5
	anchor_top = 1.0
	anchor_bottom = 1.0
	grow_horizontal = Control.GROW_DIRECTION_BOTH
	grow_vertical = Control.GROW_DIRECTION_BEGIN
	offset_bottom = -(HudWidgets.px("margin") + HudWidgets.px("dock_h") + HudWidgets.px("drawer_gap"))
	var col := HudWidgets.vbox(self)
	_head = HudWidgets.hbox(col)
	_strip = HudWidgets.hbox(col, &"Pad")
	for p in CityStore.query(&"road_profiles"):
		_profile_ids[p.id] = p.index
	catalog = RoadCatalog.new()
	catalog.profile_picked.connect(func(i: int) -> void:
		var upgrading := tools != null and tools.tool == ToolController.Tool.UPGRADE
		retool(i, "UPGRADE" if upgrading else "ROAD", "profile"))
	hide()


## The catalogue leaves the tree while another strip shows (_build_strip); a node out of the tree is not freed with
## it, so the drawer frees it then (else it outlived the game: an orphan node at exit).
func _notification(what: int) -> void:
	if what == NOTIFICATION_PREDELETE and is_instance_valid(catalog) and catalog.get_parent() == null:
		catalog.free()


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
	options.hide()
	category = {}
	closed.emit()


func select_tab(tab: Dictionary) -> void:
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
		params.zone = _zone()
	elif tab.tool == "TRANSIT":
		params = { "kind": tab.kind, "mode": tab.mode }
	elif tools and ToolController.Tool.has(tab.tool):
		params = tools.remembered.get(ToolController.Tool[tab.tool], {}).duplicate()
	tool_requested.emit(tab.tool, params)


## The tool in hand again with one parameter changed (a card or a slider of the strip).
func retool(value: Variant, tool: String, key: String) -> void:
	var params: Dictionary = tools.remembered.get(ToolController.Tool[tool], {}).duplicate() if tools else {}
	params[key] = value
	tool_requested.emit(tool, params)
	if key not in ["radius", "strength", "lanes", "width", "parking"]:
		DrawerStrips.light(_strip, value)


## A road of the catalogue by id, as if its card were clicked.
func pick_road(id: String) -> void:
	catalog.pick(_profile_ids.get(id, 0))


func _param(tool: String, key: String, fallback: Variant) -> Variant:
	return tools.last_param(ToolController.Tool[tool], key, fallback) if tools else fallback


## The zone the brush last painted (ToolController remembers it), else the first card's.
func _zone() -> int:
	var first: int = category.cards[0].zone if category.has("cards") else 0
	return tools.last_param(ToolController.Tool.ZONE, "zone", first) if tools else first


func pick_zone(zone: int) -> void:
	for c in _strip.get_children():
		if c is Button:
			c.set_pressed_no_signal(c.get_meta(&"zone", -1) == zone)
	tool_requested.emit("ZONE", {"zone": zone})


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
	if catalog.get_parent():
		catalog.get_parent().remove_child(catalog)
	HudWidgets.clear(_strip)
	var kind: String = category.get("strip", "")
	options.hide()
	if tab.get("tool", "") == "ROAD":
		options.open()
	if kind == "road_catalog" and (tab.tool == "ROAD" or tab.tool == "UPGRADE"):
		_strip.add_child(catalog)
	elif kind == "zone_cards" and tab.tool == "ZONE":
		for c in category.cards:
			_strip.add_child(_zone_card(c))
	elif tab.get("tool", "") == "ROUNDABOUT":
		var ring: Dictionary = category.ring
		DrawerStrips.slider(_strip, ring.label_key, DataTables.tuning("roundabout_radius_min_m"),
			DataTables.tuning("roundabout_radius_max_m"), ring.step, _param("ROUNDABOUT", "radius", ring.default), " m",
			retool.bind("ROUNDABOUT", "radius"))
	elif kind == "land_panels":
		LandStrips.build(self, _strip, tab.get("tool", ""))
	elif tab.get("tool", "") == "BUILD":
		BuilderStrip.fill(_strip, tab.id, tools)
	elif tab.get("kind", "") == "line":
		TransitStrip.build(_strip)
	elif kind == "build_cards" and tab.get("tool", "") == "LOT":
		for c in category.cards:
			_strip.add_child(DrawerStrips.card(c, "fn", _param("LOT", "fn", 0), retool.bind("LOT", "fn")))
	var note := HudWidgets.label(_strip, &"MutedLabel", tr(tab.get("note_key", "")) if tab.has("note_key") else "")
	note.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	note.custom_minimum_size.x = HudWidgets.px("side_w")
	note.visible = note.text != ""


## The Landscape strips' hands (ui/land_strips.gd): the tool of the open panel and its remembered parameters.
func land_param(tool_name: String, key: String, fallback: Variant) -> Variant:
	return _param(tool_name, key, fallback)


func land_slide(value: float, tool_name: String, key: String) -> void:
	retool(value, tool_name, key)


func land_pick(value: Variant, tool_name: String, key: String) -> void:
	retool(value, tool_name, key)
	_land_rebuild()


## A card of the Sky tab: another tool in hand, its panel in the strip.
func land_switch(tool_name: String) -> void:
	var params: Dictionary = tools.remembered.get(ToolController.Tool[tool_name], {}).duplicate() if tools else {}
	tool_requested.emit(tool_name, params)
	_land_rebuild()


## What wipes the whole map asks first, and only when there is something to wipe (land_panels.json "confirm").
func land_action(action: String) -> void:
	var ask: Dictionary = DataTables.ui("land_panels").confirm.get(action, {})
	if ask.is_empty():
		_land_do(action)
	elif _land_holds(ask):
		var dialog := ConfirmationDialog.new()
		dialog.dialog_text = tr(ask.key)
		dialog.confirmed.connect(_land_do.bind(action))
		dialog.visibility_changed.connect(func() -> void:
			if not dialog.visible:
				dialog.queue_free())
		add_child(dialog)
		dialog.popup_centered()


## Whether the landscape holds what the action would wipe: any of the counts named, or always when none is.
func _land_holds(ask: Dictionary) -> bool:
	var counts: Array = ask.get("counts", [])
	if counts.is_empty():
		return true
	var land: Dictionary = CityStore.query(&"landscape")
	for key: String in counts:
		var held: Variant = land[key]
		if (held.size() if held is Array else int(held)) > 0:
			return true
	return false


func _land_do(action: String) -> void:
	if action == "scatter_clouds":
		var s := BrushDefaults.of("CLOUD", tools.remembered.get(ToolController.Tool.CLOUD, {}) if tools else {})
		var like := {"height": s.height, "size": s.size, "density": s.density / PERCENT}
		var r: Dictionary = CityStore.dispatch(&"scatter_clouds", [int(s.count), like, s.variation / PERCENT])
		if r.get("error", "") != "" and tools:
			tools.hint.emit(tr(r.error), false)
	else:
		CityStore.dispatch(StringName(action))


func _land_rebuild() -> void:
	HudWidgets.clear(_strip)
	LandStrips.build(self, _strip, ToolController.Tool.find_key(tools.tool))


func _zone_card(c: Dictionary) -> Button:
	var variation := &"SwatchCard" if c.has("swatch") else &"Card"
	var b := HudWidgets.button(variation, c.get("icon", ""), tr(c.label_key), "", "card_w", "card_h")
	b.toggle_mode = true
	b.set_meta(&"zone", c.zone)
	b.set_pressed_no_signal(c.zone == _zone())
	b.vertical_icon_alignment = VERTICAL_ALIGNMENT_TOP
	b.icon_alignment = HORIZONTAL_ALIGNMENT_CENTER
	if c.has("swatch"):
		b.icon = ThemeBuilder.swatch(c.swatch, int(HudWidgets.px("card_glyph")))
	b.pressed.connect(pick_zone.bind(c.zone))
	return b
