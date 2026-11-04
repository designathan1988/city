## The drawer over the dock: the open category's tabs and a close
## button in the head, its strip below (road catalogue, zone cards or the tool's note). A tab or card asks for a tool
## with `tool_requested`; tabs whose tool the core does not have yet are drawn "being built" (ui.soon). Cards and
## sliders of the Relief, Draw and Roundabout strips change the tool's parameters (ToolController remembers them).
class_name HudDrawer
extends PanelContainer

signal tool_requested(tool: String, params: Dictionary)
signal closed

const DENSITY_FIRST := 0
const DENSITIES := [[0, "zone.low", "low"], [1, "zone.medium", "medium"], [2, "zone.high", "high"]]

## The panel at the bottom left that holds the tool's options (the HUD makes it).
@export var options_panel: PanelContainer
@onready var _options: VBoxContainer
@onready var category: Dictionary = {}
@onready var catalog: RoadCatalog
@onready var _head: HBoxContainer
@onready var _strip: Container
@onready var _tabs: Dictionary = {}
@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
@onready var _profile_ids: Dictionary = {}
@onready var _tab: Dictionary = {}
@onready var _density := DENSITY_FIRST


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
	_options = HudWidgets.vbox(options_panel, &"Pad")
	options_panel.hide()
	var col := HudWidgets.vbox(self)
	_head = HudWidgets.hbox(col)
	_strip = HudWidgets.hbox(col, &"Pad")
	for p in CityStore.query(&"road_profiles"):
		_profile_ids[p.id] = p.index
	catalog = RoadCatalog.new()
	catalog.minimum_size_changed.connect(_fit)
	get_viewport().size_changed.connect(_fit)
	catalog.profile_picked.connect(func(i: int) -> void:
		var upgrading := tools != null and tools.tool == ToolController.Tool.UPGRADE
		tool_requested.emit("UPGRADE" if upgrading else "ROAD", {"profile": i}))
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
		params.profile = catalog.picked()
	elif tab.tool == "ZONE":
		params.zone = _zone()
	elif tools and ToolController.Tool.has(tab.tool):
		params = tools.remembered.get(ToolController.Tool[tab.tool], {}).duplicate()
	tool_requested.emit(tab.tool, params)


## The tool in hand again with one parameter changed (a card or a slider of the strip).
func _retool(value: Variant, tool: String, key: String) -> void:
	var params: Dictionary = tools.remembered.get(ToolController.Tool[tool], {}).duplicate() if tools else {}
	params[key] = value
	tool_requested.emit(tool, params)
	if key != "radius" and key != "strength":
		DrawerStrips.light(_strip, value)


func _param(tool: String, key: String, fallback: Variant) -> Variant:
	return tools.last_param(ToolController.Tool[tool], key, fallback) if tools else fallback


## The zone the brush last painted (ToolController remembers it), else the first card's.
func _zone() -> int:
	var first: int = category.cards[0].zone if category.has("cards") else 0
	return tools.last_param(ToolController.Tool.ZONE, "zone", first) if tools else first


func _density_row() -> void:
	var items := []
	for d: Array in DENSITIES:
		items.append([d[0], "", d[2], "%s: %s" % [tr("v2.zone.density"), tr(d[1])]])
	var row := HudWidgets.option_row(_options, tr("v2.zone.density"))
	HudWidgets.choices(row, items, _density, func(v: int) -> void:
		_density = v
		pick_zone(DrawerStrips.use_of(_zone())))


## The Relief brush by key: `key` a step up or down, the sliders drawn again.
func step_brush(key: String, dir: int) -> void:
	var spec: Dictionary = category.brush[key]
	_retool(clampf(_param("TERRAIN", key, spec.default) + spec.step * dir, spec.min, spec.max), "TERRAIN", key)
	_build_strip(_tab)


## The Relief brush's operation by the number row; false: no such operation.
func pick_relief(index: int) -> bool:
	if index >= category.cards.size():
		return false
	_retool(category.cards[index].mode, "TERRAIN", "mode")
	return true


## The language changed: what the drawer shows, in the new one.
func relabel() -> void:
	catalog.relabel()
	if not category.is_empty() and visible:
		_build_head()
		_build_strip(_tab)


func pick_zone(zone: int) -> void:
	for c in _strip.get_children():
		if c is Button:
			c.set_pressed_no_signal(c.get_meta(&"zone", -1) == DrawerStrips.use_of(zone))
	tool_requested.emit("ZONE", {"zone": DrawerStrips.zone_at(DrawerStrips.use_of(zone), _density)})


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
	HudWidgets.clear(_options)
	var kind: String = category.get("strip", "")
	if kind == "road_catalog" and (tab.tool == "ROAD" or tab.tool == "UPGRADE"):
		_strip.add_child(catalog)
	elif kind == "zone_cards" and tab.tool == "ZONE":
		for c in category.cards:
			_strip.add_child(_zone_card(c))
		_density_row()
	elif tab.get("tool", "") == "ROUNDABOUT":
		var ring: Dictionary = category.ring
		HudWidgets.range_row(_options, tr(ring.label_key), DataTables.tuning("roundabout_radius_min_m"),
			DataTables.tuning("roundabout_radius_max_m"), ring.step, _param("ROUNDABOUT", "radius", ring.default), Formatters.format_length,
			_retool.bind("ROUNDABOUT", "radius"))
	elif kind == "relief_cards" and tab.get("tool", "") == "TERRAIN":
		_relief_strip()
	elif kind == "build_cards" and tab.get("tool", "") == "LOT":
		for c in category.cards:
			_strip.add_child(DrawerStrips.card(c, "fn", _param("LOT", "fn", 0), _retool.bind("LOT", "fn")))
	var note := HudWidgets.label(_strip, &"MutedLabel", tr(tab.get("note_key", "")) if tab.has("note_key") else "")
	note.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	note.custom_minimum_size.x = HudWidgets.px("note_w")
	note.visible = note.text != "" and _strip.get_child_count() == 1 # a mode with nothing to pick says what it does
	options_panel.visible = _options.get_child_count() > 0
	options_panel.reset_size()
	options_panel.offset_top = -options_panel.size.y - HudWidgets.px("margin")
	_fit.call_deferred()


## The room left by the options; the catalogue takes all it needs of it, the panel stands centred there, on its bottom edge.
func _fit() -> void:
	if not visible:
		return
	var margin := HudWidgets.px("margin")
	var vw := get_viewport_rect().size.x
	var left := margin + (HudWidgets.px("opts_w") + margin if options_panel.visible else 0.0)
	var room := vw - margin - left
	catalog.fit(room - HudWidgets.px("gap"))
	var need := get_combined_minimum_size()
	var width := minf(need.x, room)
	offset_left = left + (room - width) * 0.5 - vw * 0.5
	offset_right = offset_left + width
	offset_top = offset_bottom - need.y


func _relief_strip() -> void:
	for c in category.cards:
		_strip.add_child(DrawerStrips.card(c, "mode", _param("TERRAIN", "mode", 0), _retool.bind("TERRAIN", "mode")))
	for key in ["radius", "strength"]:
		var spec: Dictionary = category.brush[key]
		HudWidgets.range_row(_options, tr(spec.label_key), spec.min, spec.max, spec.step, _param("TERRAIN", key, spec.default),
			Formatters.format_length if key == "radius" else Formatters.format_points, _retool.bind("TERRAIN", key))


func _zone_card(c: Dictionary) -> Button:
	var variation := &"SwatchCard" if c.has("swatch") else &"Card"
	var b := HudWidgets.button(variation, c.get("icon", ""), tr(c.label_key), "", "card_w", "card_h")
	b.toggle_mode = true
	b.set_meta(&"zone", c.zone)
	b.set_pressed_no_signal(c.zone == DrawerStrips.use_of(_zone()))
	b.vertical_icon_alignment = VERTICAL_ALIGNMENT_TOP
	b.icon_alignment = HORIZONTAL_ALIGNMENT_CENTER
	if c.has("swatch"):
		b.icon = ThemeBuilder.swatch(c.swatch, int(HudWidgets.px("card_glyph")))
	b.pressed.connect(pick_zone.bind(c.zone))
	return b
