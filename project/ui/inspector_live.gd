## The inspector's live part (InspectorPanel): a road's or a junction's numbers as cards, two to a row, and a junction's signal
## lamps; the next reading written into the same labels, or the part built again when its rows or lamps differ in kind or number.
class_name InspectorLive
extends RefCounted

const LAMP_DOTS := [&"DotGo", &"DotSlow", &"DotBroken"]

## What the live part shows: each row's name key and value label, each lamp's dot and label.
var _row_keys: Array = []
var _values: Array[Label] = []
var _lamp_dots: Array[Panel] = []
var _lamp_texts: Array[Label] = []


## The numbers as cards, two to a row; the live ones' (`live`) labels are kept for the next numbers.
func rows(col: Control, list: Array, live := false) -> void:
	var g := GridContainer.new()
	g.columns = 2
	g.theme_type_variation = &"StatGrid"
	col.add_child(g)
	for r: Array in list:
		var card := HudWidgets.panel(g, &"StatCard")
		card.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		var v := HudWidgets.vbox(card)
		HudWidgets.label(v, &"StatName", col.tr(r[0]))
		var value := HudWidgets.label(v, &"StatValue", r[1])
		if live:
			_row_keys.append(r[0])
			_values.append(value)


func forget() -> void:
	_row_keys = []
	_values = []
	_lamp_dots = []
	_lamp_texts = []


func fill_road(col: Control, list: Array) -> void:
	forget()
	rows(col, list, true)


## A junction's rows and lamps, from one reading of its traffic.
func fill_node(col: Control, traffic: Dictionary) -> void:
	forget()
	rows(col, InspectorRows.node_rows(traffic), true)
	var chips := HFlowContainer.new()
	col.add_child(chips)
	for lamp: Array in InspectorRows.lamps(traffic):
		var row := HudWidgets.hbox(HudWidgets.panel(chips, &"LampChip"))
		var dot := Panel.new()
		dot.theme_type_variation = LAMP_DOTS[lamp[1]]
		dot.custom_minimum_size = Vector2.ONE * HudWidgets.px("dot")
		dot.size_flags_vertical = Control.SIZE_SHRINK_CENTER
		row.add_child(dot)
		_lamp_dots.append(dot)
		_lamp_texts.append(HudWidgets.label(row, &"MetaLabel", lamp[0]))


## The live numbers written into the labels already shown; false when the rows or the lamps are not the same ones.
func write(list: Array, lamps: Array) -> bool:
	if list.size() != _row_keys.size() or lamps.size() != _lamp_dots.size():
		return false
	for i in list.size():
		if list[i][0] != _row_keys[i]:
			return false
	for i in list.size():
		_values[i].text = list[i][1]
	for i in lamps.size():
		_lamp_dots[i].theme_type_variation = LAMP_DOTS[lamps[i][1]]
		_lamp_texts[i].text = lamps[i][0]
	return true
