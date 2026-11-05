## The Models shelf of the Build drawer: a search box, the kinds of building in one list and a card with a picture for each model of
## the catalogue and of the player's own. A card puts the model in hand; a selected building can be kept as a model of the player's.
## What is shown comes from its own controls and the store each time: the shelf keeps nothing.
class_name ModelShelf
extends VBoxContainer

const ALL := "all"
const MINE := "mine"
const STUDIO_NAME := &"ModelStudio"
const KEY_META := &"model_key"

@export var tools: ToolController

@onready var _head: HBoxContainer = HudWidgets.hbox(self)
@onready var _row: HBoxContainer = HudWidgets.hbox(self)
@onready var _search: LineEdit = _line("builder.search")
@onready var _groups: OptionButton = _group_list()
@onready var _name_box: LineEdit = _line("building.blueprintName")
@onready var _studio: ModelStudio = _studio_of()


static func fill(col: Container, controller: ToolController) -> void:
	var shelf := ModelShelf.new()
	shelf.tools = controller
	col.add_child(shelf)


func _ready() -> void:
	_studio.pictured.connect(_show_picture)
	_search.text_changed.connect(func(_text: String) -> void: _show_cards())
	_groups.item_selected.connect(func(_i: int) -> void: _show_cards())
	var keep := HudWidgets.button(&"Card", "m_save", tr("builder.tool.saveModel"), "", "card_w", "tab_h")
	keep.pressed.connect(_keep)
	_head.add_child(keep)
	_show_cards()


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
	return out


func _bytes(entry: Dictionary) -> PackedByteArray:
	return entry.bytes if entry.has("bytes") else CityStore.query(&"model_bytes", [entry.key])


func _show_cards() -> void:
	HudWidgets.clear(_row)
	var group: String = DataTables.ui("builder").model_groups[_groups.selected]
	var query := _search.text.strip_edges().to_lower()
	for e in _entries():
		if (group == ALL or e.group == group) and (query == "" or e.label.to_lower().contains(query)):
			_row.add_child(_card(e))
	if _row.get_child_count() == 0:
		HudWidgets.label(_row, &"MutedLabel", tr("builder.search.none"))


func _card(entry: Dictionary) -> Control:
	var b := HudWidgets.button(&"Card", "models", entry.label, "", "card_w", "card_h")
	b.vertical_icon_alignment = VERTICAL_ALIGNMENT_TOP
	b.icon_alignment = HORIZONTAL_ALIGNMENT_CENTER
	b.expand_icon = true
	b.set_meta(KEY_META, entry.key)
	b.pressed.connect(_choose.bind(entry))
	_studio.ask.call_deferred(entry.key, _bytes(entry))
	if entry.group != MINE:
		return b
	var col := VBoxContainer.new()
	col.add_child(b)
	var drop := HudWidgets.button(&"IconButton", "close", "", "builder.city.remove", "icon_btn", "tab_h")
	drop.pressed.connect(_drop.bind(entry))
	col.add_child(drop)
	return col


func _show_picture(key: String, picture: Texture2D) -> void:
	for card in _row.find_children("*", "Button", true, false) + _row.get_children():
		if card is Button and card.get_meta(KEY_META, "") == key:
			card.icon = picture


func _choose(entry: Dictionary) -> void:
	if tools.active is BuildingTool:
		tools.active.take({"bytes": _bytes(entry), "function": entry.function, "zone": entry.zone})


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
		_show_cards()


func _drop(entry: Dictionary) -> void:
	if not ModelLibrary.remove(entry.key):
		tools.hint.emit(tr("building.blueprintNotRemoved"), false)
		return
	ModelStudio.pictures.erase(entry.key)
	_show_cards()
