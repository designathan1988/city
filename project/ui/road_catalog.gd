## The road catalogue in the Roads drawer: category tabs with counts and a
## row of cards (picture, name, "{width} m · {price}/m"); a click picks the profile to draw with. Width and price per
## metre are the core's derived numbers (query road_profiles); the strips for the picture come from the catalogue.
class_name RoadCatalog
extends VBoxContainer

signal profile_picked(index: int)

@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
@onready var _profiles: Array = []
@onready var _strips: Dictionary = {}
@onready var _tabs_row: HBoxContainer
@onready var _cards_row: HBoxContainer
@onready var _tab_buttons: Dictionary = {}


func _ready() -> void:
	for p in DataTables.road_profiles():
		_strips[p.id] = p.strips
	for p in CityStore.query(&"road_profiles"):
		if not p.ring:
			p["strips"] = _strips.get(p.id, [])
			_profiles.append(p)
	var strip := HudWidgets.panel(self, &"TabStrip")
	strip.size_flags_horizontal = Control.SIZE_SHRINK_BEGIN
	_tabs_row = HudWidgets.hbox(strip)
	var counts := {}
	for p in _profiles:
		counts[p.category] = counts.get(p.category, 0) + 1
	for t in DataTables.ui("drawers").catalog_tabs:
		var count: int = counts.get(t[0], 0)
		var b := HudWidgets.button(&"RcTab", "", "%s %d" % [tr(t[1]), count], "", "", "tab_h")
		b.toggle_mode = true
		b.pressed.connect(show_category.bind(t[0]))
		_tabs_row.add_child(b)
		_tab_buttons[t[0]] = b
	var scroll := ScrollContainer.new()
	scroll.vertical_scroll_mode = ScrollContainer.SCROLL_MODE_DISABLED
	scroll.custom_minimum_size.x = HudWidgets.px("drawer_max_w")
	add_child(scroll)
	_cards_row = HudWidgets.hbox(scroll, &"Pad")
	show_category(DataTables.ui("drawers").catalog_tabs[0][0])


func show_category(category: String) -> void:
	for k in _tab_buttons:
		_tab_buttons[k].set_pressed_no_signal(k == category)
	HudWidgets.clear(_cards_row)
	var widest := 0.0
	for p in _profiles:
		widest = maxf(widest, p.width)
	for p in _profiles:
		if p.category == category:
			_cards_row.add_child(_card(p, widest))


## The profile the road tool last drew with (ToolController remembers it).
func picked() -> int:
	return tools.last_param(ToolController.Tool.ROAD, "profile", 0) if tools else 0


func pick(index: int) -> void:
	for c in _cards_row.get_children():
		c.get_meta(&"button").set_pressed_no_signal(c.get_meta(&"index") == index)
	profile_picked.emit(index)


func _card(p: Dictionary, widest: float) -> Control:
	var col := VBoxContainer.new()
	col.custom_minimum_size.x = HudWidgets.px("rc_card_w")
	var b := HudWidgets.button(&"RcCard", "", "", "", "rc_card_w", "rc_art_h")
	b.toggle_mode = true
	b.icon = RoadSwatch.texture(p, widest)
	b.expand_icon = true
	b.set_pressed_no_signal(p.index == picked())
	b.tooltip_text = tr("catalog.tip").format({"name": tr(p.name_key),
		"lanes": tr("palette.lanes.count.other").format({"count": p.lanes}),
		"way": tr("catalog.oneWay") if p.one_way else tr("catalog.twoWay"), "speed": int(p.speed)})
	b.pressed.connect(pick.bind(p.index))
	col.add_child(b)
	col.set_meta(&"button", b)
	col.set_meta(&"index", p.index)
	var name := HudWidgets.label(col, &"CardLabel", tr(p.name_key))
	name.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	name.custom_minimum_size.x = HudWidgets.px("rc_card_w")
	HudWidgets.label(col, &"MetaLabel", tr("catalog.meta").format({
		"width": "%.1f" % p.width, "price": Formatters.format_money(p.cost_per_m)}))
	return col
