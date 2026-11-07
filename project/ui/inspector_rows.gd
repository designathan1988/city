## The inspector's live numbers, as the rows it shows: a road's and a junction's, from the core's queries. Holds nothing; the panel asks
## again every refresh_s (data/game/health.json is the panels' pace).
class_name InspectorRows
extends RefCounted

const CONTROL_KEYS := ["control.none", "control.priority", "control.signal"]
const LIGHT_KEYS := ["signal.green", "signal.amber", "signal.red"]
const NO_VALUE := "—"
const CLEARING := 1 # the lamp between green and red


static func _speed(ms: float) -> String:
	return NO_VALUE if ms < 0.0 else Formatters.format_speed(ms)


static func _traffic(t: Dictionary) -> Array:
	return [["inspector.meanSpeed", _speed(t.mean_speed_ms)], ["inspector.vehicles", Formatters.format_int(t.vehicles)],
		["inspector.queue", Formatters.format_int(t.queued)], ["inspector.capacity", Formatters.count_label("inspector.capacityValue", t.capacity)],
		["inspector.volume", Formatters.format_int(t.volume)]]


## Length, lanes, limit, then what the traffic does there.
static func segment_rows(info: Dictionary) -> Array:
	var t: Dictionary = CityStore.query(&"segment_traffic", [info.id])
	if t.is_empty():
		return []
	var lanes := tr_key("inspector.lanesOneWay" if info.one_way else "inspector.lanesTwoWay").format(
			{"count": info.lanes, "perSide": info.lanes / 2})
	var rows := [["inspector.length", Formatters.format_length_tenths(info.length_m)], ["inspector.lanes", lanes],
		["inspector.speed", Formatters.format_speed(info.speed_ms)]]
	rows.append_array(_traffic(t))
	return rows


## A junction's traffic (query node_traffic), asked once for its rows and its lamps.
static func node_traffic(id: int) -> Dictionary:
	return CityStore.query(&"node_traffic", [id])


## Control, legs and movements, the traffic; with a signal, its cycle and stage (`t`: node_traffic).
static func node_rows(t: Dictionary) -> Array:
	if t.is_empty():
		return []
	var control := tr_key("control.disconnected") if not t.known else tr_key(CONTROL_KEYS[t.control])
	var rows := [["inspector.control", control], ["inspector.connections", str(t.connections)], ["inspector.movements", str(t.get("movements", 0))],
		["inspector.vehicles", Formatters.format_int(t.vehicles)], ["inspector.queue", Formatters.format_int(t.queued)],
		["inspector.meanSpeed", _speed(t.mean_speed_ms)], ["inspector.capacity", Formatters.count_label("inspector.capacityValue", t.capacity)],
		["inspector.volume", Formatters.format_int(t.volume)]]
	if t.has("cycle_s"):
		var light: String = LIGHT_KEYS[CLEARING if t.get("lamps", []).has(CLEARING) else 0]
		rows.append(["inspector.cycle", "%d s" % int(round(t.cycle_s))])
		rows.append(["inspector.phase", "%d/%d · %s" % [int(t.stage) + 1, int(t.stages), tr_key(light)]])
	return rows


## Each signal stage as a lamp: [text, light] (`t`: node_traffic).
static func lamps(t: Dictionary) -> Array:
	var out := []
	var lights: Array = t.get("lamps", [])
	for i in lights.size():
		out.append(["%s · %s" % [tr_key("inspector.group").format({"id": i + 1}).to_upper(), tr_key(LIGHT_KEYS[lights[i]])], lights[i]])
	return out


static func tr_key(key: String) -> String:
	return TranslationServer.translate(key)
