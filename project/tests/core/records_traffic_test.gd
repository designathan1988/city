## The traffic's own tables kept true when a record goes and its id is taken again before the traffic drains the change log
## (doc/specs/003-roadcraft-features/sheets/backbone-records-true.md item 8): a lot's car gate keeps bays off its street's kerb; the
## lot is demolished and another building, far away, takes its id with no tick between; the kerb must come back where the lot
## stood, read by the box the change log kept, not by the record now at the id. Items 9 to 12 and 15 are steps of yard_test and
## flow_control_test.
## Run: godot --path project res://tests/core/records_traffic_test.tscn
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const OUT_M := 1500.0 # the street this far past the town: past park_window_out_m (traffic.json 1100) from every lot of it
const STREET_M := 300.0
const PARKING := 1 # road_profiles.json streetWithParking: bays along both kerbs
const PX := 0.25
const CAMERA_UP_M := 30.0
const HOP_M := 80.0 # the camera steps this far each read, past park_window_step_m (traffic.json 64): the parking looks round again
const READ_STEPS := 60
const STEP_TICKS := 20
const STEADY_READS := 3 # the bay count unchanged this many reads running: every source near the camera read, the far ones let go
const LOT_FRONT_M := 8.0 # the lot's front this far from the street's line: past its half width (7 m), within kerb_gate_reach_m (8 m)
## The lots tried, [frontage along the street, depth]. A street reads the gates of the lots whose centre lies within its box grown by
## its half width and kerb_gate_reach_m (lot_access.cpp street_gates): 7 + 7 + 8 = 22 m from its line here, so the depth stays at
## most 2 * (22 - LOT_FRONT_M) = 28 m; the body a lot grows (and whether it has a car gate) follows its size and place.
const LOT_SIZES := [[40.0, 28.0], [60.0, 28.0], [80.0, 28.0], [40.0, 26.0], [60.0, 26.0]]
const FAR_M := 1500.0 # the other building this far again: past park_window_m (900), never read
const FAR_LOT_M := 20.0
const ZONE_RES := 1
## Lots tried until one has a car gate on its front: [function, zone] (BuildingFn, zones.json order).
const LOT_KINDS := [[0, 3], [8, 2], [0, 2], [0, 1]]
const EL_CAR_GATE := 10 # b40 Element
const EL_STALL := 18
const EL_DOCK := 19


func _ready() -> void:
	var camera := Camera3D.new()
	add_child(camera)
	camera.make_current()
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	await _kerbs_after_lot_reuse(camera)
	finish()


func _kerbs_after_lot_reuse(camera: Camera3D) -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var mid := Vector3(b.max.x + OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	# the camera over free ground until the town's lots and kerbs are let go: every bay counted below is this site's
	var read := {}
	for i in READ_STEPS:
		camera.global_position = mid + Vector3(0, CAMERA_UP_M, HOP_M * (i % 2))
		await get_tree().process_frame
		ticks(STEP_TICKS)
		read = CityStore.query(&"traffic_probe", [0])
		if read.lots_read == 0 and read.kerbs_read == 0:
			break
	if not check(read.lots_read == 0 and read.kerbs_read == 0, "the town's bays let go behind the camera: %d lots, %d kerbs read" % [read.lots_read, read.kerbs_read]):
		return
	var built: Dictionary = CityStore.dispatch(&"build_road", [mid - Vector3(0, 0, STREET_M * 0.5), mid + Vector3(0, 0, STREET_M * 0.5), false,
			Vector3.ZERO, PARKING, PX])
	if not check(built.get("ok", false), "a street with parking laid out of the town: %s" % built):
		return
	var n0: int = await _settled_bays(camera, mid)
	if not check(n0 > 0, "its kerb bays read: %d" % n0):
		return
	var lot := _lot_with_car_gate(mid)
	if not check(lot.id >= 0, "a lot with a car gate on the street drawn"):
		return
	var n1: int = await _settled_bays(camera, mid)
	check(n1 - lot.bays < n0, "the lot's car gate keeps kerb bays off: %d of the %d kerb bays left (its own %d beside)" % [n1 - lot.bays, n0, lot.bays])
	# the lot goes and a building far away takes its id, no tick between: the traffic drains both changes at once
	var gone: Dictionary = CityStore.dispatch(&"demolish", [lot.centre])
	check(gone.get("ok", false), "the lot demolished: %s" % gone)
	var far := mid + Vector3(FAR_M, 0, 0)
	var other: Dictionary = CityStore.dispatch(&"draw_building", [far, far + Vector3(FAR_LOT_M, 0, FAR_LOT_M), 0, ZONE_RES, 0])
	check(other.get("ok", false) and int(other.get("placed", -1)) == lot.id, "the far building took the lot's id %d: %s" % [lot.id, other])
	var n2: int = await _settled_bays(camera, mid)
	check(CityStore.query(&"traffic_probe", [0]).lots_read == 0, "no lot read near the street: the far building is out of the window")
	check(n2 == n0, "the kerb comes back where the lot stood: %d bays, %d before the lot" % [n2, n0])
	var broken := _broken("parking.lot_sources_live")
	check(broken.is_empty(), "every lot read is an alive building's, its bays on it: %s" % [broken])


## The bays read near the camera once their count holds: the camera hops along the street so the parking looks round each read.
func _settled_bays(camera: Camera3D, mid: Vector3) -> int:
	var last := -1
	var steady := 0
	for i in READ_STEPS:
		camera.global_position = mid + Vector3(0, CAMERA_UP_M, HOP_M * (i % 2))
		await get_tree().process_frame
		ticks(STEP_TICKS)
		var bays: int = CityStore.query(&"traffic_probe", [0]).bays
		steady = steady + 1 if bays == last else 0
		last = bays
		if steady >= STEADY_READS:
			break
	return last


## A lot drawn on the street's side, of the first size and kind whose body has a car gate (the others undone): its id, its centre and
## its own bays (stalls and docks), or id -1.
func _lot_with_car_gate(mid: Vector3) -> Dictionary:
	for size: Array in LOT_SIZES:
		var a := mid + Vector3(LOT_FRONT_M, 0, -size[0] * 0.5)
		var e := a + Vector3(size[1], 0, size[0])
		for kind: Array in LOT_KINDS:
			var r: Dictionary = CityStore.dispatch(&"draw_building", [a, e, kind[0], kind[1], 0])
			if not r.get("ok", false):
				continue
			var id: int = r.placed
			var gates := 0
			var bays := 0
			for row: Array in CityStore.query(&"lot_elements", [id]):
				gates += 1 if row[0] == EL_CAR_GATE else 0
				bays += 1 if row[0] == EL_STALL or row[0] == EL_DOCK else 0
			print("lot ", id, " size ", size, " kind ", kind, " car gates ", gates, " bays ", bays)
			if gates > 0:
				return {"id": id, "centre": (a + e) * 0.5, "bays": bays}
			CityStore.dispatch(&"undo")
	return {"id": -1}


## The records the core's invariant `name` (feature.check) finds broken now.
func _broken(name: String) -> Array:
	var out := []
	for b: Dictionary in CityStore.query(&"broken_records"):
		if "%s.%s" % [b.feature, b.check] == name:
			for r in b.records:
				out.append(r)
	return out
