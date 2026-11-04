## The strip of the Make a line tab: each line with its vehicles, fewer, more, and a way to remove it.
class_name TransitStrip
extends RefCounted


static func build(parent: Control) -> void:
	HudWidgets.clear(parent)
	var plan: Dictionary = CityStore.query(&"transit_plan")
	for line: Dictionary in plan.get("lines", []):
		var row := HudWidgets.hbox(parent)
		HudWidgets.label(row, &"MutedLabel", "%s  %d" % [line.name, line.vehicles])
		_button(row, "-", _change.bind(parent, line.id, line.vehicles - 1))
		_button(row, "+", _change.bind(parent, line.id, line.vehicles + 1))
		_button(row, "x", _drop.bind(parent, line.id))


static func _button(row: Control, text: String, action: Callable) -> void:
	var b := Button.new()
	b.text = text
	b.pressed.connect(action)
	row.add_child(b)


static func _change(parent: Control, id: int, vehicles: int) -> void:
	CityStore.dispatch(&"transit_vehicles", [id, vehicles])
	build(parent)


static func _drop(parent: Control, id: int) -> void:
	CityStore.dispatch(&"transit_drop_line", [id])
	build(parent)
