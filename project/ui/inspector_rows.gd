## The inspector's live numbers, as the rows it shows: a road's and a junction's, from the core's queries, each given to the live
## part (InspectorLive.row, .lamp) as shown. Holds nothing; the panel asks again every inspector_refresh_s (data/ui/hud.json timings).
class_name InspectorRows
extends RefCounted

const CONTROL_KEYS := { &"NONE": "control.none", &"PRIORITY": "control.priority", &"SIGNAL": "control.signal" } # by the core's Control
const LIGHT_KEYS := ["signal.green", "signal.amber", "signal.red"]
const NO_VALUE := "—"
const CLEARING := 1 # the lamp between green and red


static func _speed(ms: float) -> String:
	return NO_VALUE if ms < 0.0 else Formatters.format_speed(ms)


static func _traffic(t: Dictionary, live: InspectorLive) -> void:
	live.row("inspector.meanSpeed", _speed(t.mean_speed_ms))
	live.row("inspector.vehicles", Formatters.format_int(t.vehicles))
	live.row("inspector.queue", Formatters.format_int(t.queued))
	live.row("inspector.capacity", Formatters.count_label("inspector.capacityValue", t.capacity))
	live.row("inspector.volume", Formatters.format_int(t.volume))


## Length, lanes, limit, then what the traffic does there.
static func segment_rows(info: Dictionary, live: InspectorLive) -> void:
	var t: Dictionary = CityStore.query(&"segment_traffic", [info.id])
	if t.is_empty():
		return
	live.row("inspector.length", Formatters.format_length_tenths(info.length_m))
	live.row("inspector.lanes", tr_key("inspector.lanesOneWay" if info.one_way else "inspector.lanesTwoWay").format(
			{"count": info.lanes, "perSide": info.lanes / 2}))
	live.row("inspector.speed", Formatters.format_speed(info.speed_ms))
	_traffic(t, live)


## A junction's traffic (query node_traffic), asked once for its rows and its lamps.
static func node_traffic(id: int) -> Dictionary:
	return CityStore.query(&"node_traffic", [id])


## Control, legs and movements, the traffic; with a signal, its cycle and stage (`t`: node_traffic).
static func node_rows(t: Dictionary, live: InspectorLive) -> void:
	if t.is_empty():
		return
	live.row("inspector.control", tr_key("control.disconnected") if not t.known else tr_key(CONTROL_KEYS[CityStore.code_name(&"Control", int(t.control))]))
	live.row("inspector.connections", Formatters.format_int(t.connections))
	live.row("inspector.movements", Formatters.format_int(t.get("movements", 0)))
	live.row("inspector.vehicles", Formatters.format_int(t.vehicles))
	live.row("inspector.queue", Formatters.format_int(t.queued))
	live.row("inspector.meanSpeed", _speed(t.mean_speed_ms))
	live.row("inspector.capacity", Formatters.count_label("inspector.capacityValue", t.capacity))
	live.row("inspector.volume", Formatters.format_int(t.volume))
	if t.has("cycle_s"):
		var light: String = LIGHT_KEYS[CLEARING if t.get("lamps", []).has(CLEARING) else 0]
		live.row("inspector.cycle", tr_key("unit.second").format({"n": int(round(t.cycle_s))}))
		live.row("inspector.phase", "%d/%d · %s" % [int(t.stage) + 1, int(t.stages), tr_key(light)])


## Each signal stage as a lamp, its text and its light (`t`: node_traffic).
static func lamps(t: Dictionary, live: InspectorLive) -> void:
	var lights: Array = t.get("lamps", [])
	for i in lights.size():
		live.lamp("%s · %s" % [tr_key("inspector.group").format({"id": i + 1}).to_upper(), tr_key(LIGHT_KEYS[lights[i]])], lights[i])


static func tr_key(key: String) -> String:
	return TranslationServer.translate(key)
