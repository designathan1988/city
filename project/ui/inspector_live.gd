## The inspector's live part (InspectorPanel): a road's or a junction's numbers as cards, two to a row, and a junction's signal
## lamps, each given by InspectorRows one at a time (row, lamp); the next reading written into the same labels, or the part built
## again when its rows or lamps differ in kind or number.
class_name InspectorLive
extends RefCounted

const LAMP_DOTS := [&"DotGo", &"DotSlow", &"DotBroken"]

## What the live part shows: each row's name key and value label, each lamp's dot and label.
var _row_keys: Array = []
var _values: Array[Label] = []
var _lamp_dots: Array[Panel] = []
var _lamp_texts: Array[Label] = []
## Where the rows and lamps given next go: the grid and the lamp row being built; with neither, the labels shown, from _next on.
var _grid: GridContainer
var _chips: HFlowContainer
var _keep := false
var _next := 0
var _next_lamp := 0
var _same := true


## The cards of the rows given next (row), two to a row, in col; the live ones' (`live`) labels are kept for the next numbers.
func cards(col: Control, live := false) -> void:
	_grid = GridContainer.new()
	_grid.columns = 2
	_grid.theme_type_variation = &"StatGrid"
	col.add_child(_grid)
	_chips = null
	_keep = live


func forget() -> void:
	_row_keys = []
	_values = []
	_lamp_dots = []
	_lamp_texts = []


func fill_road(col: Control, info: Dictionary) -> void:
	forget()
	cards(col, true)
	InspectorRows.segment_rows(info, self)


## A junction's rows and lamps, from one reading of its traffic.
func fill_node(col: Control, traffic: Dictionary) -> void:
	forget()
	cards(col, true)
	InspectorRows.node_rows(traffic, self)
	_chips = HFlowContainer.new()
	col.add_child(_chips)
	InspectorRows.lamps(traffic, self)


## One row, its name key and its value: a card while the part is built, else written into the label shown for that row.
func row(key: String, text: String) -> void:
	if _grid != null:
		var card := HudWidgets.panel(_grid, &"StatCard")
		card.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		var v := HudWidgets.vbox(card)
		HudWidgets.label(v, &"StatName", _grid.tr(key))
		var value := HudWidgets.label(v, &"StatValue", text)
		if _keep:
			_row_keys.append(key)
			_values.append(value)
		return
	_same = _same and _next < _row_keys.size() and _row_keys[_next] == key
	if _same:
		_values[_next].text = text
	_next += 1


## One signal lamp, its text and its light: a chip while the part is built, else written into the chip shown.
func lamp(text: String, light: int) -> void:
	if _chips != null:
		var chip := HudWidgets.hbox(HudWidgets.panel(_chips, &"LampChip"))
		var dot := Panel.new()
		dot.theme_type_variation = LAMP_DOTS[light]
		dot.custom_minimum_size = Vector2.ONE * HudWidgets.px("dot")
		dot.size_flags_vertical = Control.SIZE_SHRINK_CENTER
		chip.add_child(dot)
		_lamp_dots.append(dot)
		_lamp_texts.append(HudWidgets.label(chip, &"MetaLabel", text))
		return
	_same = _same and _next_lamp < _lamp_dots.size()
	if _same:
		_lamp_dots[_next_lamp].theme_type_variation = LAMP_DOTS[light]
		_lamp_texts[_next_lamp].text = text
	_next_lamp += 1


## A road's live numbers written into the labels already shown; false when the rows are not the same ones.
func write_road(info: Dictionary) -> bool:
	_start_writing()
	InspectorRows.segment_rows(info, self)
	return _written()


## A junction's live numbers and lamps written into the labels already shown; false when they are not the same ones.
func write_node(traffic: Dictionary) -> bool:
	_start_writing()
	InspectorRows.node_rows(traffic, self)
	InspectorRows.lamps(traffic, self)
	return _written()


func _start_writing() -> void:
	_grid = null
	_chips = null
	_next = 0
	_next_lamp = 0
	_same = true


func _written() -> bool:
	return _same and _next == _row_keys.size() and _next_lamp == _lamp_dots.size()
