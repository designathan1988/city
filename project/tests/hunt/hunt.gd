## The defect hunt: one run of the generated town that asks the core's probes for every defect the player would see and writes
## them, each with where it stands and the code that owns its mechanism, to one JSON (--out=<path>; tools/hunt/hunt.py runs it
## hidden and prints the list). What it asks:
## - vehicles standing in one another (probe vehicle_bodies, every tick of a watch round the camera), each pair classed by the
##   state of its two bodies (on a lane, on a connector, a lot car, a lorry, a body not yet whole on its lane) into the mechanism
##   that put them there, with that mechanism's file;
## - the street's pieces in one another, on the ground and in volume (probe bodies_audit);
## - furniture chunks whose wires are drawn at no level or at two (probe wire_draws: a pole with no wires);
## - road surfaces in one another (probe road_overlaps).
## Run: godot --path project res://tests/hunt/hunt.tscn -- --no-flow --out=<path>
extends TestCase

const TOWN_MAP := 0
const SEED := 7
const HOUR := 8.0 # the morning's traffic
const WARM_TICKS := 1200
const WATCH_TICKS := 900
const VIEW_M := 400.0
const AT := Vector3.ZERO # the window's middle: the camera of a test stands at the origin
const STREET_GAP_M := 0.0 # bodies_audit: pieces in one another, not nearer than a gap
const CASES_KEPT := 12 # the first cases of each class written out whole
const SETTLE_FRAMES := 60
const SETTLE_LIMIT_S := 60.0
# the probe's row of a body (native/src/api/city_core_clearance_probe.cpp body_row)
const R_ID := 0
const R_X := 1
const R_Z := 2
const R_OFF := 4
const R_TYPE := 5
const R_ON_CONN := 6
const R_DS := 7
const R_LANE := 8
const R_S := 9
const R_CAR := 16
const R_PHASE := 17
const R_LORRY := 20
## The classes of two vehicles in one another: [why, the files owning that mechanism].
const OWNERS := {
	"lorry": ["a lorry swinging into or out of its yard over lanes nobody closed for it",
		"native/src/sim/traffic/lorry_traffic.cpp, native/src/sim/traffic/lane_closures.cpp"],
	"lot_cars": ["two lot cars on a lot's ways at once (sent out together, or over the other bays)",
		"native/src/sim/traffic/lot_demand.cpp, native/src/sim/buildings/lot_access.cpp"],
	"lot_car_lane": ["a lot car crossing the kerb while a lane's follower does not see it",
		"native/src/sim/traffic/lot_traffic.cpp"],
	"lane_mouth": ["a body not yet whole on its lane (handed over or put at its start) standing in the junction",
		"native/src/sim/traffic/meso.cpp, native/src/sim/traffic/window.cpp, native/src/sim/traffic/lane_graph_edit.cpp, native/src/sim/traffic/vehicle_pose.cpp"],
	"connectors": ["two bodies crossing a junction on connectors that conflict",
		"native/src/sim/traffic/junction_admission.cpp"],
	"connector_lane": ["a body coming off a connector onto a lane where another stands",
		"native/src/sim/traffic/vehicle_pose.cpp, native/src/sim/traffic/junction_admission.cpp"],
	"same_lane": ["a follower on its leader's lane closer than its body",
		"native/src/sim/traffic/vehicle_motion.cpp"],
	"lane_change": ["a lane change onto a lane where another stands",
		"native/src/sim/traffic/lane_change.cpp"],
	"other": ["state not classed", "native/src/sim/traffic"],
}

const TRAFFIC := "res://data/catalog/traffic.json"
const CLASS_LENGTH := 6 # a class row's length (traffic.json _classes_doc)

var _out := ""
var _defects: Array = []
var _lengths: Array = [] # each vehicle type's length, metres


func _ready() -> void:
	for arg: String in OS.get_cmdline_user_args():
		if arg.begins_with("--out="):
			_out = arg.trim_prefix("--out=")
	if not boot():
		finish()
		return
	for row: Array in JSON.parse_string(FileAccess.get_file_as_string(TRAFFIC)).classes:
		_lengths.append(float(row[CLASS_LENGTH]))
	new_map(TOWN_MAP, SEED, true)
	Settings.attach(world, get_viewport())
	_street()
	_roads()
	await _wires()
	_vehicles()
	_write()
	check(not _out.is_empty(), "the hunt wrote its list: --out=<path> given")
	finish()


func _defect(kind: String, why: String, owner: String, count: int, where: Variant, cases: Array) -> void:
	if count <= 0:
		return
	_defects.append({"kind": kind, "why": why, "owner": owner, "count": count, "where": where, "cases": cases})


func _street() -> void:
	var a: Dictionary = CityStore.query(&"bodies_audit", [STREET_GAP_M])
	print("HUNT %d street pieces audited" % int(a.bodies))
	check(int(a.bodies) > 0, "the hunt saw the street's pieces: %d" % int(a.bodies))
	for kinds: Vector2i in a.by_kinds:
		_defect("street_pieces_on_ground", "two pieces of the street standing in one another on the ground (kinds %d and %d)" % [kinds.x, kinds.y],
				"native/src/sim/street (street_furniture placement)", int(a.by_kinds[kinds]), null, [a.cases[kinds]])
	for kinds: Vector2i in a.volume_by_kinds:
		_defect("street_pieces_in_volume", "a part of one piece passing through a part of another (kinds %d and %d)" % [kinds.x, kinds.y],
				"native/src/sim/street (street_furniture placement)", int(a.volume_by_kinds[kinds]), null, [a.volume_cases[kinds]])


func _roads() -> void:
	var r: Dictionary = CityStore.query(&"road_overlaps")
	for key: String in ["overlaps", "pairs", "count"]:
		if r.has(key) and typeof(r[key]) == TYPE_INT:
			_defect("road_surfaces", "two road surfaces standing in one another", "native/src/render/roads", int(r[key]), r.get("first", null), [])
			return


func _wires() -> void:
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	await StateWait.drawn(get_tree(), SETTLE_LIMIT_S)
	var none: Array = []
	var two: Array = []
	var with_spans := 0
	for row: Dictionary in CityStore.query(&"wire_draws"):
		if int(row.spans) == 0:
			continue
		with_spans += 1
		var levels := int(bool(row.near)) + int(bool(row.far))
		if levels == 0:
			none.append(row)
		elif levels == 2:
			two.append(row)
	print("HUNT %d furniture chunks with spans drawn" % with_spans)
	check(with_spans > 0, "the hunt saw the town's lines drawn: %d chunks with spans" % with_spans)
	_defect("wires_not_drawn", "a chunk's wires drawn at no level: poles with no wires", "native/src/render/props/prop_renderer.cpp pick",
			none.size(), null, none.slice(0, CASES_KEPT))
	_defect("wires_two_levels", "a chunk's wires drawn at two levels at once", "native/src/render/props/prop_renderer.cpp pick",
			two.size(), null, two.slice(0, CASES_KEPT))


func _vehicles() -> void:
	CityStore.query(&"set_city_hour", [HOUR])
	ticks(WARM_TICKS)
	var by_class := {}
	var seen := {}
	var bodies := 0
	for i in WATCH_TICKS:
		ticks(1)
		var watch: Dictionary = CityStore.query(&"vehicle_bodies", [AT, VIEW_M])
		bodies += watch.bodies.size()
		if int(watch.in_another) == 0 or watch.deepest_pair.size() < 2:
			continue
		var a: Array = watch.deepest_pair[0]
		var b: Array = watch.deepest_pair[1]
		var key := "%d-%d" % [mini(int(a[R_ID]), int(b[R_ID])), maxi(int(a[R_ID]), int(b[R_ID]))]
		if seen.has(key):
			continue # one pair counted once, at its first tick
		seen[key] = true
		var cls := _class_of(a, b)
		if not by_class.has(cls):
			by_class[cls] = []
		by_class[cls].append({"tick": i, "depth_m": watch.deepest_m, "at": [a[R_X], a[R_Z]], "a": a, "b": b})
	print("HUNT watched %d ticks, %d bodies seen in all" % [WATCH_TICKS, bodies])
	check(bodies > 0, "the hunt saw vehicles round the camera: %d bodies over %d ticks" % [bodies, WATCH_TICKS])
	for cls: String in by_class:
		var cases: Array = by_class[cls]
		_defect("vehicles_in_one_another:" + cls, OWNERS[cls][0], OWNERS[cls][1], cases.size(), cases[0].at, cases.slice(0, CASES_KEPT))


## The mechanism two bodies standing in one another come from, read off their states.
func _class_of(a: Array, b: Array) -> String:
	var lorry := int(a[R_LORRY]) >= 0 or int(b[R_LORRY]) >= 0
	var off_a := bool(a[R_OFF])
	var off_b := bool(b[R_OFF])
	if lorry:
		return "lorry"
	if off_a and off_b:
		return "lot_cars"
	if off_a or off_b:
		return "lot_car_lane"
	var conn_a := bool(a[R_ON_CONN])
	var conn_b := bool(b[R_ON_CONN])
	if conn_a and conn_b:
		return "connectors"
	if _not_whole(a) or _not_whole(b):
		return "lane_mouth"
	if conn_a or conn_b:
		return "connector_lane"
	if int(a[R_DS]) == int(b[R_DS]):
		return "same_lane" if int(a[R_LANE]) == int(b[R_LANE]) else "lane_change"
	return "other"


## A body on a lane whose front is less than its own length in: its rear is still behind the lane's start.
func _not_whole(r: Array) -> bool:
	if bool(r[R_ON_CONN]) or bool(r[R_OFF]):
		return false
	return float(r[R_S]) < float(_lengths[int(r[R_TYPE])])


func _write() -> void:
	_defects.sort_custom(func(x: Dictionary, y: Dictionary) -> bool: return int(x.count) > int(y.count))
	print("HUNT %d defect classes" % _defects.size())
	for d: Dictionary in _defects:
		print("  %5d  %-40s %s  ->  %s" % [d.count, d.kind, d.why, d.owner])
	if _out.is_empty():
		return
	var f := FileAccess.open(_out, FileAccess.WRITE)
	if f == null:
		check(false, "the hunt's list could not be written to %s" % _out)
		return
	f.store_string(JSON.stringify({"map": TOWN_MAP, "seed": SEED, "defects": _defects}, " "))
