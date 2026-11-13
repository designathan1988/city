## Where a look plan stands the camera: the player's own rig, at the plan's view of a point or of somebody (the walker nearest the map's
## middle, the clock stopped). The rig goes on living, so it keeps its tilt within the player's, takes its lens from the distance and
## keeps the eye out of the buildings and over the ground: a photo shows what a player standing there sees, and nothing a player cannot.
class_name LookAim
extends RefCounted

const WALKERS_READ := 4096
const RIGHT_ANGLE := 90.0 # a plan's bearing runs from +x towards +z, the rig's from +z towards +x
const STAND_FRAMES := 2 # the frames the world takes to ask for what the new view shows
const BUILDING_REACH_M := 60.0 # how far round the point setViewNear looks for a building: a town block and its streets
const PAIR_REACH_M := 40.0 # how far round a plan's point it looks for two walkers together (LookArgs pair): a street and its corner
## The street furniture's kinds as the core numbers them (native/src/sim/catalog/furniture_types.h, FurnitureKind).
const FURNITURE := {"lamp": 1, "pole": 11, "sign": 12}
## A pole carries wires: a utility pole, or a lamp of a street's main row, whose poles are wired (sim/roads/street_furniture.h, row).
const WIRED_ROW := 0


## `view` is the setView's zoom, azimuth and elevation: the walker nearest the map's middle.
static func stand_on_walker(rig: CityCamera, ui: UiDriver, view: Array) -> void:
	await stand_near(rig, ui, ["walker", 0.0, 0.0, view[0], view[1], view[2]])


## setViewNear [kind, x, y, zoom, azimuth, elevation]: the plan's view of the element of `kind` nearest (x, y) (y is the map's north, -z),
## found by the core: "walker" or "vehicle" (shot_targets, the clock stopped so it stays in the frame, a fallen walker skipped); "fallen"
## (a walker on the ground, tripped or knocked down: its body is drawn where it fell); "body" (a body the effects draw, the dead included:
## effects_body_places); "pair" (the two bodies lying nearest one another, wherever they lie: the middle between them; the point is not read);
## "lamp", "pole" or "sign" (the street furniture of the street under the vehicle nearest the point: furniture_spots of segment_at, the
## player's own pole runs too for "pole"; a plan runs the clock first so traffic is out); "building" (the tallest within
## BUILDING_REACH_M, CityCore.highest_roof, aimed at its foot). False, said, with none of that kind there.
static func stand_near(rig: CityCamera, ui: UiDriver, args: Array) -> bool:
	var kind := str(args[0])
	var from := Vector2(float(args[1]), -float(args[2]))
	var found := _nearest(kind, from)
	if found == Vector3.INF:
		print("FAIL no %s near %s" % [kind, [args[1], args[2]]])
		return false
	print("%s at %s" % [kind, found])
	await stand(rig, ui, found, [args[3], args[4], args[5]])
	return true


## Where the element of `kind` nearest `from` (x, z) stands, or Vector3.INF.
static func _nearest(kind: String, from: Vector2) -> Vector3:
	var points: Array = []
	match kind:
		"lamp", "pole", "sign":
			points = _street_furniture(kind, from)
		"walker", "vehicle":
			CityStore.dispatch(&"set_speed", [0])
			for t: Dictionary in CityStore.query(&"shot_targets", [WALKERS_READ])[kind + "s"]:
				if not t.get("fallen", false):
					points.push_back(t.position)
		"body":
			for at: Vector3 in CityStore.query(&"effects_body_places"):
				points.push_back(at)
		"pair":
			points = nearest_pair(Array(CityStore.query(&"effects_body_places")), from, INF)
		"fallen":
			CityStore.dispatch(&"set_speed", [0])
			for t: Dictionary in CityStore.query(&"shot_targets", [WALKERS_READ]).walkers:
				if t.get("fallen", false):
					points.push_back(t.position)
		"building":
			var roof: Dictionary = CityStore.query(&"highest_roof", [from.x, from.y, BUILDING_REACH_M])
			if roof.get("hit", false):
				points.push_back(Vector3(roof.position.x, 0.0, roof.position.z)) # its foot: the rig stands on the ground there
	return _closest(points, from)


## The furniture of `kind` on the first street that has it, of the streets under the vehicles from the nearest to `from` outwards
## (and, for poles, the player's pole runs).
static func _street_furniture(kind: String, from: Vector2) -> Array:
	var points: Array = Array(CityStore.query(&"street_poles")) if kind == "pole" else []
	CityStore.dispatch(&"set_speed", [0])
	var cars: Array = CityStore.query(&"shot_targets", [WALKERS_READ]).vehicles.map(func(v: Dictionary) -> Vector3: return v.position)
	cars.sort_custom(func(a: Vector3, b: Vector3) -> bool: return Vector2(a.x, a.z).distance_squared_to(from) < Vector2(b.x, b.z).distance_squared_to(from))
	for car: Vector3 in cars:
		var under: Dictionary = CityStore.query(&"segment_at", [car])
		for spot: Dictionary in CityStore.query(&"furniture_spots", [int(under.get("seg", -1))]):
			if int(spot.kind) == FURNITURE[kind] or (kind == "pole" and int(spot.kind) == FURNITURE.lamp and int(spot.row) == WIRED_ROW):
				points.push_back(spot.position)
		if not points.is_empty():
			return points
	return points


## The middle between the two of `points` (bodies, walkers) within `reach` metres of `from` (x, z) that stand nearest one another, alone in
## an array; empty with fewer than two.
static func nearest_pair(points: Array, from: Vector2, reach := PAIR_REACH_M) -> Array:
	var near := points.filter(func(p: Vector3) -> bool: return Vector2(p.x, p.z).distance_to(from) < reach)
	var best := INF
	var middle: Array = []
	for i in near.size():
		for j in range(i + 1, near.size()):
			var d: float = near[i].distance_squared_to(near[j])
			if d < best:
				best = d
				middle = [(near[i] + near[j]) * 0.5]
	return middle


## Of `points` (walkers), the one with the most others within `crowd` metres: the heart of a crowd. Vector3.INF with none.
static func busiest(points: Array, crowd: float) -> Vector3:
	var best := Vector3.INF
	var most := -1
	for p: Vector3 in points:
		var round_it := points.filter(func(o: Vector3) -> bool: return o.distance_to(p) < crowd).size()
		if round_it > most:
			most = round_it
			best = p
	return best


static func _closest(points: Array, from: Vector2) -> Vector3:
	var best := Vector3.INF
	for p: Vector3 in points:
		if best == Vector3.INF or Vector2(p.x, p.z).distance_squared_to(from) < Vector2(best.x, best.z).distance_squared_to(from):
			best = p
	return best


## setViewRoof [x, y, radius, zoom, azimuth, elevation]: the plan's view of the highest roof within `radius` metres of (x, y) (y is the map's
## north, -z), found by the core (CityCore.highest_roof): the ground under a town's centre is its streets, its roofs tens of metres over them.
## The rig's own point stays on the ground (the arm never lifts it onto a roof), so the view is held on the roof itself (CityCamera.pin):
## the eye where the rig's orbit at that zoom, bearing and tilt would stand round that point, the rig's lens for that zoom. False, said,
## with no roof there.
static func stand_on_roof(rig: CityCamera, ui: UiDriver, args: Array) -> bool:
	var roof: Dictionary = CityStore.query(&"highest_roof", [float(args[0]), -float(args[1]), float(args[2])])
	if not roof.get("hit", false):
		print("FAIL no roof within %s m of %s" % [args[2], [args[0], args[1]]])
		return false
	var at: Vector3 = roof.position
	var half := _half_of(rig, float(args[3]))
	var yaw := deg_to_rad(RIGHT_ANGLE - float(args[4]))
	var eye := at + Basis.from_euler(Vector3(-deg_to_rad(float(args[5])), yaw, 0.0)).z * CameraProfile.view_distance(rig.cfg, half)
	print("roof at ", at)
	CameraProfile.lens(rig.camera, rig.cfg, half)
	rig.pin(eye, at)
	await ui.frames(STAND_FRAMES)
	return true


## The half height of the ground in view for a zoom in screen pixels a metre, within the rig's.
static func _half_of(rig: CityCamera, zoom: float) -> float:
	return clampf(rig.get_viewport().get_visible_rect().size.y / (2.0 * zoom), rig.cfg.view_half_min, rig.cfg.view_half_max)


## A plan's view of the point `p`: `view` is the setView's zoom (screen pixels a metre at the point looked at), azimuth and elevation
## (degrees). The zoom is the half height of the ground in view there, which the rig's distance and lens follow from.
static func stand(rig: CityCamera, ui: UiDriver, p: Vector3, view: Array) -> void:
	var half := _half_of(rig, float(view[0]))
	rig.unpin() # a view pinned before (a pair's camera) goes back to the rig
	rig.jump(p, RIGHT_ANGLE - float(view[1]), float(view[2]), CameraProfile.view_distance(rig.cfg, half))
	await ui.frames(STAND_FRAMES)
