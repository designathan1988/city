## Where the capture lays the transport: the 4.0's stops stand on a street with open ground across it, and its photos are taken from
## there, so a street like that here gives the same poses, not a mirror or another frame. A footway whose every view has a clear line.
class_name CaptureTransitSite
extends RefCounted

const SCAN_M := 30.0
const REACH_M := 25.0
const SAME_STREET := 0.9 # the cosine between two stops' directions: still the same street
const APART_M := 40.0
const FARTHEST_M := 200.0
const STEP_M := 10.0
const OPEN_FROM_M := 14.0 # past the street's footway
const OPEN_TO_M := 60.0 # as far as the crossing's photo sees the train
const OPEN_STEP_M := 7.0
const TRAIN_TOP_M := 4.0

## What the viewport's last frame drew, visible and in the shadows.
static func drawn(viewport: RID) -> String:
	var parts: Array[String] = []
	for kind: int in [RenderingServer.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, RenderingServer.VIEWPORT_RENDER_INFO_TYPE_SHADOW]:
		parts.append("%d triangles, %d draw calls" % [RenderingServer.viewport_get_render_info(viewport, kind, RenderingServer.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME),
				RenderingServer.viewport_get_render_info(viewport, kind, RenderingServer.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME)])
	return "visible %s; shadow %s" % parts


## A view from a place: {at, along, out}; view = [along, out, up, lens].
static func eye_of(place: Dictionary, view: Array) -> Vector3:
	var at: Vector3 = place.at
	var along: Vector3 = place.along
	var out: Vector3 = place.out
	return at + along * float(view[0]) + out * float(view[1]) + Vector3.UP * float(view[2])


static func sees(place: Dictionary, view: Array) -> bool:
	var eye := eye_of(place, view)
	return CityStore.query(&"camera_clear", [eye, CaptureParity.CAMERA_CLEAR_M, false]) and CaptureParity.line_clear(eye, place.at)


## Nothing a train would run through along `along` from where it crosses the street at `road`: the side the crossing's photo sees.
static func open_run(road: Vector3, along: Vector3) -> bool:
	var metres := OPEN_FROM_M
	while metres <= OPEN_TO_M:
		var over: Vector3 = road + along * metres + Vector3.UP * TRAIN_TOP_M
		if not CityStore.query(&"camera_clear", [over, CaptureParity.CAMERA_CLEAR_M, false]):
			return false
		metres += OPEN_STEP_M
	return true


## The footway `metres` along from `place`, on the same street; empty when it ends before.
static func along_street(place: Dictionary, metres: float) -> Dictionary:
	var at: Vector3 = place.at
	var along: Vector3 = place.along
	var there: Dictionary = CityStore.query(&"transit_footway", [at + along * metres, REACH_M])
	if there.is_empty() or absf((there.along as Vector3).dot(along)) < SAME_STREET:
		return {}
	return there


## Every footway of the town on the grid of the scan, farthest from the middle first: the open ground is at the edge.
static func footways() -> Array:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var middle: Vector3 = (b.min + b.max) / 2
	var spots: Array = []
	var x: float = b.min.x
	while x <= b.max.x:
		var z: float = b.min.z
		while z <= b.max.z:
			var r: Dictionary = CityStore.query(&"transit_footway", [Vector3(x, 0, z), REACH_M])
			if not r.is_empty():
				spots.append(r)
			z += SCAN_M
		x += SCAN_M
	spots.sort_custom(func(p: Dictionary, q: Dictionary) -> bool: return p.at.distance_to(middle) > q.at.distance_to(middle))
	return spots


## The first street with a footway where `views` are clear, and along it a clear place for each of `others`, apart from the rest:
## {"place", "others": [places], "third"}. Empty when no street has them.
static func street(views: Array, others: Array) -> Dictionary:
	var scanned := footways()
	for place: Dictionary in scanned:
		if not views.all(func(v: Array) -> bool: return sees(place, v)):
			continue
		var taken: Array[Vector3] = [place.at]
		var found: Array = []
		for view: Array in others:
			var there := _clear_along(place, view, taken)
			if there.is_empty():
				break
			found.append(there)
			taken.append(there.at)
		var third := _clear_along(place, [], taken)
		if found.size() == others.size() and not third.is_empty():
			return {"place": place, "others": found, "third": third}
	print("transit: no street with the other side free at ", scanned.size(), " pavements")
	return {}


## The nearest place along the street, either way, with `view` clear ([]: any) and `APART_M` from those taken.
static func _clear_along(place: Dictionary, view: Array, taken: Array[Vector3]) -> Dictionary:
	var metres := APART_M
	while metres <= FARTHEST_M:
		for sign: float in [1.0, -1.0]:
			var there := along_street(place, metres * sign)
			if there.is_empty() or not taken.all(func(t: Vector3) -> bool: return t.distance_to(there.at) >= APART_M):
				continue
			if view.is_empty() or sees(there, view):
				return there
		metres += STEP_M
	return {}
