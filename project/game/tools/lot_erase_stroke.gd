## The Lots tab's delete stroke: the points it passed and the player's lots under them (drawn thicker until it is let go); let go,
## the core takes those lots and every building under the stroke, on a lot or not (lot_erase_path), one undo step.
class_name LotEraseStroke
extends RefCounted

## The stroke is sampled at this step between two pointer reports, shorter than any lot's front.
const STEP_M := 2.0

var points := PackedVector3Array()
var _lots := {} # the lots it passed, by id: their rings


func start(p: Vector3) -> void:
	points = PackedVector3Array([p])
	_lots = {}
	_take(p)


## The whole way from the last point to `p`.
func reach(p: Vector3) -> void:
	var last := points[-1]
	var steps := ceili(last.distance_to(p) / STEP_M)
	for k in range(1, steps + 1):
		var at := last.lerp(p, float(k) / steps)
		points.push_back(at)
		_take(at)


## The rings of the lots it passed.
func rings() -> Array:
	return _lots.values()


func send() -> Dictionary:
	return CityStore.dispatch(&"lot_erase_path", [points])


func clear() -> void:
	points = PackedVector3Array()
	_lots = {}


func _take(p: Vector3) -> void:
	var id: int = CityStore.query(&"lot_at", [p])
	if id != 0 and not _lots.has(id):
		_lots[id] = CityStore.query(&"lot_ring", [id])
