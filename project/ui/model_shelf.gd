## The Models shelf of the Build drawer: a search box, the kinds of building in one list and a card with a picture for each model of
## the catalogue and of the player's own. A card puts the model in hand; a selected building can be kept as a model of the player's.
## The cards are made when the shelf opens and again when the player's own models change; the search and the kind only show or hide them.
class_name ModelShelf
extends VBoxContainer

const ALL := "all"
const MINE := "mine"
const STUDIO_NAME := &"ModelStudio"

@export var tools: ToolController

@onready var _head: HBoxContainer = HudWidgets.hbox(self)
@onready var _scroll: ScrollContainer = _scroller()
@onready var _row: GridContainer = _grid()
var _room := 0.0 # what the drawer leaves, last it said
@onready var _search: LineEdit = _line("builder.search")
@onready var _groups: OptionButton = _group_list()
@onready var _name_box: LineEdit = _line("building.blueprintName")
@onready var _studio: ModelStudio = _studio_of()
var _cards: Array[Dictionary] = [] # each model in the catalogue's order: {entry, item (what stands in the grid)}
var _by_key := {} # a model's key -> the button that shows its picture
var _none: Label # "nothing found", shown when no card is


static func fill(col: Container, controller: ToolController) -> void:
	var shelf := ModelShelf.new()
	shelf.tools = controller
	col.add_child(shelf)


func _ready() -> void:
	add_to_group(HudDrawer.FITS)
	_studio.pictured.connect(_show_picture)
	_search.text_changed.connect(func(_text: String) -> void: _show_cards())
	_groups.item_selected.connect(func(_i: int) -> void: _show_cards())
	var keep := HudWidgets.button(&"Card", "m_save", tr("builder.tool.saveModel"), "", "card_w", "tab_h")
	keep.pressed.connect(_keep)
	_head.add_child(keep)
	_make_cards()


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


static func _bytes(entry: Dictionary) -> PackedByteArray:
	return entry.bytes if entry.has("bytes") else CityStore.query(&"model_bytes", [entry.key])


## What gives the studio a model's bytes when it photographs it: nothing of the shelf's, which may be closed by then.
static func _bytes_later(entry: Dictionary) -> Callable:
	return func() -> PackedByteArray: return ModelShelf._bytes(entry)


## A card for every model, in the catalogue's order, then the filter: when the shelf opens and when the player's models change.
func _make_cards() -> void:
	HudWidgets.clear(_row)
	_cards = []
	_by_key = {}
	for e in _entries():
		var item := _card(e)
		_row.add_child(item)
		_cards.append({"entry": e, "item": item})
	_none = HudWidgets.label(_row, &"MutedLabel", tr("builder.search.none"))
	_show_cards()


## The cards of the kind chosen whose name holds the search, the others hidden (a grid lays out only what shows).
func _show_cards() -> void:
	var group: String = DataTables.ui("builder").model_groups[_groups.selected]
	var query := _search.text.strip_edges().to_lower()
	var shown := 0
	for c: Dictionary in _cards:
		var e: Dictionary = c.entry
		var on: bool = (group == ALL or e.group == group) and (query == "" or e.label.to_lower().contains(query))
		c.item.visible = on
		shown += 1 if on else 0
	_none.visible = shown == 0
	_row.columns = maxi(1, ceili(maxi(shown, 1) / float(LandStrips.CARD_ROWS)))
	fit.call_deferred(_room)


func _card(entry: Dictionary) -> Control:
	var b := HudWidgets.button(&"Card", "models", entry.label, "", "card_w", "card_h")
	b.vertical_icon_alignment = VERTICAL_ALIGNMENT_TOP
	b.icon_alignment = HORIZONTAL_ALIGNMENT_CENTER
	b.expand_icon = true
	b.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	_by_key[entry.key] = b
	b.pressed.connect(_choose.bind(entry))
	_studio.ask.call_deferred(entry.key, _bytes_later(entry))
	if entry.group != MINE:
		return b
	var col := VBoxContainer.new()
	col.add_child(b)
	var drop := HudWidgets.button(&"IconButton", "close", "", "builder.city.remove", "icon_btn", "tab_h")
	drop.pressed.connect(_drop.bind(entry))
	col.add_child(drop)
	return col


func _show_picture(key: String, picture: Texture2D) -> void:
	var card: Button = _by_key.get(key)
	if card != null:
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
		_make_cards()


func _drop(entry: Dictionary) -> void:
	if not ModelLibrary.remove(entry.key):
		tools.hint.emit(tr("building.blueprintNotRemoved"), false)
		return
	ModelStudio.pictures.erase(entry.key)
	_make_cards()
