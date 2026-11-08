## What shows over the map (the Layers popover): the ground's grid and the core's overlays, each on or off, persisted by Settings.
## The zones show with their tool in hand, and, with the layer on, where the player zoned once it is put down (main.ts showLots).
class_name MapLayers
extends RefCounted

## A layer was switched by the player: Settings saves.
signal changed

var layers: Dictionary = {} # layer -> on
var zone_tool := false # the zone brush is in hand
var _zoned := false # the player has zoned sides on this city, as last shown
var _switches: RenderSwitches


func _init(saved: Dictionary, switches: RenderSwitches) -> void:
	layers = saved
	_switches = switches


func layer_on(name: String) -> bool:
	return layers.get(name, false)


## A row of the Layers popover: the grid is the ground's, the others the core's overlays.
func set_layer(name: String, on: bool) -> void:
	layers[name] = on
	show(name)
	changed.emit()


func show(name: String) -> void:
	if name == "grid":
		_switches.set_global(&"map_grid", 1.0 if layer_on(name) else 0.0)
	elif name == "zones":
		CityStore.dispatch(&"show_every_zone", [zone_tool])
		CityStore.dispatch(&"set_overlay", [name, zone_tool or (layer_on(name) and _zoned)])
	else:
		CityStore.dispatch(&"set_overlay", [name, layer_on(name)])


func show_all() -> void:
	for name: String in layers:
		show(name)


func set_zone_tool(on: bool) -> void:
	zone_tool = on
	show("zones")


## Each frame: the zones layer follows whether the player has zoned.
func follow_zones() -> void:
	var zoned: bool = CityStore.snapshot.get("player_zones", 0) > 0
	if zoned != _zoned:
		_zoned = zoned
		show("zones")
