## The strip of the Make a line tab: each line with its colour, its mode and stops, its vehicles, fewer, more, and a way to remove it.
class_name TransitStrip
extends RefCounted

const MODE_KEYS := { &"BUS": "transit.mode.bus", &"TRAIN": "transit.mode.train", &"METRO": "transit.mode.metro" } # by the core's TransitMode


static func build(parent: Control) -> void:
	_fill(HudWidgets.vbox(parent))


## The list again, and again when the plan changes, for as long as it shows.
static func _fill(list: Control) -> void:
	if not is_instance_valid(list) or not list.is_inside_tree():
		return
	HudWidgets.clear(list)
	var tools: ToolController = list.get_tree().get_first_node_in_group(&"tools")
	if tools:
		tools.plan_changed.connect(_fill.bind(list), CONNECT_ONE_SHOT)
	var plan: Dictionary = CityStore.query(&"transit_plan")
	if plan.get("lines", []).is_empty():
		HudWidgets.label(list, &"MutedLabel", TranslationServer.translate("transit.noLines"))
	for line: Dictionary in plan.get("lines", []):
		var row := HudWidgets.hbox(list)
		var swatch := ColorRect.new()
		swatch.color = line.colour
		swatch.custom_minimum_size = Vector2.ONE * HudWidgets.px("transit_swatch")
		row.add_child(swatch)
		HudWidgets.verbatim(HudWidgets.label(row, &""), line.name) # the core's code for the line
		var stops := TranslationServer.translate("transit.stops").format({"n": line.stops.size()})
		HudWidgets.label(row, &"MutedLabel", "%s · %s" % [TranslationServer.translate(MODE_KEYS[CityStore.code_name(&"TransitMode", int(line.mode))]), stops])
		_button(row, "minus", "transit.fewer", _change.bind(list, line.id, line.vehicles - 1))
		HudWidgets.label(row, &"", Formatters.format_int(line.vehicles))
		_button(row, "plus", "transit.more", _change.bind(list, line.id, line.vehicles + 1))
		_button(row, "demolish", "transit.remove", _drop.bind(list, line.id))


static func _button(row: Control, icon: String, tip: String, action: Callable) -> void:
	var b := HudWidgets.button(&"", icon, "", tip, "transit_button", "transit_button")
	b.pressed.connect(action)
	row.add_child(b)


static func _changed(parent: Control) -> void:
	var tools: ToolController = parent.get_tree().get_first_node_in_group(&"tools")
	if tools:
		tools.plan_changed.emit()


static func _change(parent: Control, id: int, vehicles: int) -> void:
	CityStore.dispatch(&"transit_vehicles", [id, vehicles])
	_changed(parent)


static func _drop(parent: Control, id: int) -> void:
	CityStore.dispatch(&"transit_drop_line", [id])
	_changed(parent)
