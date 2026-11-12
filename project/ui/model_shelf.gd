## The Models shelf of the Build drawer: a search box, the kinds of building in one list and a card with a picture for each model of
## the catalogue and of the player's own. A card puts the model in hand; a selected building can be kept as a model of the player's.
## Catalog metadata follows library mutations; search only projects it. Thumbnail work follows the visible cards.
class_name ModelShelf
extends VBoxContainer

const ALL := "all"
const MINE := "mine"
const STUDIO_NAME := &"ModelStudio"
const KEY_META := &"model_key"
const PICTURE_META := &"pictured_key"

@export var tools: ToolController

@onready var _head: HBoxContainer = HudWidgets.hbox(self)
@onready var _scroll: ScrollContainer = _scroller()
@onready var _row: GridContainer = _grid()
@onready var _room := 0.0 # what the drawer leaves, last it said
@onready var _search: LineEdit = _line("builder.search")
@onready var _groups: OptionButton = _group_list()
@onready var _name_box: LineEdit = _line("building.blueprintName")
@onready var _studio: ModelStudio = _studio_of()
@onready var _parked := Node.new() # filtered cards stay parent-owned without rebuilding their controls
var _catalog: Array[Dictionary] = []
var _entries_by_key: Dictionary = {}
var _cards: Dictionary = {}
var _buttons: Dictionary = {}
var _shown_keys: Array[String] = []
var _picture_keys: Dictionary = {}
var _empty: Label
var _layout_ready := false
var _generation := -1


static func fill(col: Container, controller: ToolController) -> void:
	var shelf := ModelShelf.new()
	shelf.tools = controller
	col.add_child(shelf)


func _ready() -> void:
	add_to_group(HudDrawer.FITS)
	add_child(_parked, false, Node.INTERNAL_MODE_BACK)
	_studio.pictured.connect(_show_picture)
	_generation = CityStore.snapshot.get("city_generation", -1)
	CityStore.watch(self, ["city_generation"], func(snapshot: Dictionary) -> void:
		var generation: int = snapshot.get("city_generation", -1)
		if generation != _generation:
			_generation = generation
			_request_visible.call_deferred())
	_search.text_changed.connect(func(_text: String) -> void: _show_cards())
	_groups.item_selected.connect(func(_i: int) -> void: _show_cards())
	var keep := HudWidgets.button(&"Card", "m_save", tr("builder.tool.saveModel"), "", "card_w", "tab_h")
	keep.pressed.connect(_keep)
	_head.add_child(keep)
	_row.sort_children.connect(func() -> void:
		_layout_ready = true
		_request_visible.call_deferred())
	_scroll.resized.connect(func() -> void: _request_visible.call_deferred())
	_scroll.get_h_scroll_bar().value_changed.connect(func(_value: float) -> void: _request_visible())
	visibility_changed.connect(func() -> void: _request_visible.call_deferred())
	_reload_entries()
	_show_cards()


func _exit_tree() -> void:
	if is_instance_valid(_studio):
		_studio.release(get_instance_id())


func _scroller() -> ScrollContainer:
	var scroll := ScrollContainer.new()
	scroll.vertical_scroll_mode = ScrollContainer.SCROLL_MODE_DISABLED
	add_child(scroll)
	return scroll


func _grid() -> GridContainer:
	var grid := GridContainer.new()
	_scroll.add_child(grid)
	return grid


## The width the drawer leaves for the cards (HudDrawer.FITS).
func fit(width: float) -> void:
	_room = width
	_scroll.custom_minimum_size.x = minf(width, _row.get_combined_minimum_size().x)


func _line(hint_key: String) -> LineEdit:
	var box := LineEdit.new()
	box.placeholder_text = tr(hint_key)
	box.custom_minimum_size.x = HudWidgets.px("side_w")
	_head.add_child(box)
	return box


func _group_list() -> OptionButton:
	var list := OptionButton.new()
	for g: String in DataTables.ui("builder").model_groups:
		list.add_item(tr("builder.city.%s" % g))
	_head.add_child(list)
	return list


func _studio_of() -> ModelStudio:
	var studio: ModelStudio = tools.get_node_or_null(NodePath(STUDIO_NAME))
	if studio == null:
		studio = ModelStudio.new()
		studio.name = STUDIO_NAME
		tools.add_child(studio)
	return studio


func _entries() -> Array[Dictionary]:
	var out: Array[Dictionary] = []
	for m: Dictionary in CityStore.query(&"model_list"):
		out.append({"key": m.id, "label": tr("model.%s" % m.id), "group": m.group, "function": m.function, "zone": m.zone})
	for m in ModelLibrary.list():
		out.append({"key": m.key, "label": m.name, "group": MINE, "function": m.function, "zone": m.zone, "bytes": m.bytes})
	out.append_array(LotProjects.entries())
	return out


func _bytes(entry: Dictionary) -> PackedByteArray:
	return entry.bytes if entry.has("bytes") else CityStore.query(&"model_bytes", [entry.key])


func _reload_entries() -> void:
	_catalog.clear()
	_entries_by_key.clear()
	for original: Dictionary in _entries():
		var entry := original.duplicate()
		entry.view_key = "catalog:" + entry.key # catalog bodies are immutable during this studio's play session
		if entry.has("bytes"):
			var hash := HashingContext.new()
			hash.start(HashingContext.HASH_SHA256)
			hash.update(entry.bytes)
			entry.view_key = "%s:%s:%s" % [entry.group, entry.key, hash.finish().hex_encode()]
		_catalog.push_back(entry)
		_entries_by_key[entry.view_key] = entry
	for key: String in _cards.keys():
		if _entries_by_key.has(key):
			_buttons[key].text = _entries_by_key[key].label
			continue
		var card: Control = _cards[key]
		card.get_parent().remove_child(card)
		card.queue_free()
		_cards.erase(key)
		_buttons.erase(key)
		_studio.invalidate(key)


func _show_cards() -> void:
	var group: String = DataTables.ui("builder").model_groups[_groups.selected]
	var query := _search.text.strip_edges().to_lower()
	var shown: Array[String] = []
	for e in _catalog:
		if (group == ALL or e.group == group) and (query == "" or e.label.to_lower().contains(query)):
			shown.push_back(e.view_key)
	if shown == _shown_keys and (_empty != null or not shown.is_empty()):
		_request_visible.call_deferred()
		return
	var selected := {}
	for key in shown:
		selected[key] = true
	for key in _shown_keys:
		if selected.has(key) or not _cards.has(key):
			continue
		var card: Control = _cards[key]
		_row.remove_child(card)
		_parked.add_child(card)
		card.hide()
		_clear_picture(key)
	if _empty != null:
		_row.remove_child(_empty)
		_empty.queue_free()
		_empty = null
	_shown_keys = shown
	_layout_ready = false
	for i in shown.size():
		var key := shown[i]
		if not _cards.has(key):
			_cards[key] = _card(_entries_by_key[key])
		var card: Control = _cards[key]
		if card.get_parent() != _row:
			if card.get_parent() != null:
				card.get_parent().remove_child(card)
			_row.add_child(card)
		card.show()
		_row.move_child(card, i)
	if shown.is_empty():
		_empty = HudWidgets.label(_row, &"MutedLabel", tr("builder.search.none"))
	_row.columns = maxi(1, ceili(shown.size() / float(LandStrips.CARD_ROWS)))
	_studio.set_wanted(get_instance_id(), shown)
	fit.call_deferred(_room)
	_request_visible.call_deferred()


func _card(entry: Dictionary) -> Control:
	var b := HudWidgets.button(&"Card", "models", entry.label, "", "card_w", "card_h")
	b.vertical_icon_alignment = VERTICAL_ALIGNMENT_TOP
	b.icon_alignment = HORIZONTAL_ALIGNMENT_CENTER
	b.expand_icon = true
	b.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	b.set_meta(KEY_META, entry.view_key)
	b.pressed.connect(_choose_key.bind(entry.view_key))
	_buttons[entry.view_key] = b
	if entry.group != MINE:
		return b
	var col := VBoxContainer.new()
	col.add_child(b)
	var drop := HudWidgets.button(&"IconButton", "close", "", "builder.city.remove", "icon_btn", "tab_h")
	drop.pressed.connect(_drop.bind(entry))
	col.add_child(drop)
	return col


func _show_picture(key: String, picture: Texture2D) -> void:
	if _picture_keys.has(key) and _buttons.has(key):
		var button: Button = _buttons[key]
		button.icon = picture
		button.set_meta(PICTURE_META, key)


func _clear_picture(key: String) -> void:
	if not _buttons.has(key):
		return
	var button: Button = _buttons[key]
	button.icon = HudWidgets.icon("models")
	if button.has_meta(PICTURE_META):
		button.remove_meta(PICTURE_META)


## Grid columns are sorted in X. Locate the first visible column by binary search, then visit its visible rows only.
func _request_visible() -> void:
	if not is_inside_tree():
		return
	if is_visible_in_tree() and not _shown_keys.is_empty() and not _layout_ready:
		return # the container's deferred sort publishes the new card positions before viewport selection
	var keys: Array[String] = []
	var visible_keys := {}
	if is_visible_in_tree() and not _shown_keys.is_empty():
		var clip := _scroll.get_global_rect()
		var columns := _row.columns
		var lo := 0
		var hi := columns
		while lo < hi:
			var mid := (lo + hi) / 2
			var end := _row.get_global_rect().end.x
			if mid + 1 < columns:
				var next: Control = _row.get_child(mid + 1)
				end = next.get_global_rect().position.x
			if end < clip.position.x:
				lo = mid + 1
			else:
				hi = mid
		var column := lo
		while column < columns:
			var first: Control = _row.get_child(column)
			if first.get_global_rect().position.x > clip.end.x:
				break
			for row in LandStrips.CARD_ROWS:
				var index := row * columns + column
				if index >= _shown_keys.size():
					break
				var card: Control = _row.get_child(index)
				if card.get_global_rect().intersects(clip):
					var key := _shown_keys[index]
					keys.push_back(key)
					visible_keys[key] = true
			column += 1
	for key: String in _picture_keys:
		if not visible_keys.has(key):
			_clear_picture(key)
	_picture_keys = visible_keys
	_studio.set_wanted(get_instance_id(), keys)
	for key in keys:
		var button: Button = _buttons[key]
		if button.get_meta(PICTURE_META, "") == key:
			continue # visible button already owns its picture, even if the retention cache evicted it
		var picture := _studio.peek(key)
		if picture != null:
			_show_picture(key, picture)
		elif _studio.needs(key):
			_studio.ask(key, _bytes(_entries_by_key[key]))


func _choose(entry: Dictionary) -> void:
	if tools.active is BuildingTool:
		tools.active.take({"bytes": _bytes(entry), "function": entry.function, "zone": entry.zone})


func _choose_key(key: String) -> void:
	if _entries_by_key.has(key):
		_choose(_entries_by_key[key])


func _keep() -> void:
	var text := _name_box.text.strip_edges()
	var picked: Dictionary = tools.active.picked if tools.active is BuildingTool else {}
	if text == "" or picked.is_empty():
		tools.hint.emit(tr("hint.builder.select" if picked.is_empty() else "building.blueprintName"), false)
		return
	var saved := ModelLibrary.save(text, CityStore.query(&"building_bytes", [picked.id]))
	tools.hint.emit(tr("building.blueprintSaved" if saved else "building.blueprintNotSaved"), false)
	if saved:
		_name_box.clear()
		_reload_entries()
		_show_cards()


func _drop(entry: Dictionary) -> void:
	if not ModelLibrary.remove(entry.key):
		tools.hint.emit(tr("building.blueprintNotRemoved"), false)
		return
	_studio.invalidate(entry.view_key)
	_reload_entries()
	_show_cards()
