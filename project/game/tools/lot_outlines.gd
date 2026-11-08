## The player's lots as the Lots tab draws them (query lots, read again after each edit): each lot's id, zone and outline on the
## ground, and what of them is under the pointer: a lot by its id, the corner nearest a point, the side of a lot nearest it.
class_name LotOutlines
extends RefCounted

var all: Array = [] # {id, zone, points}, as the core gives them


## The lots read again from the core.
func read() -> void:
	all = CityStore.query(&"lots")


func of(id: int) -> Dictionary:
	for l: Dictionary in all:
		if l.id == id:
			return l
	return {}


## The corner of any lot nearest `p` within `reach` metres, or INF.
func corner_near(p: Vector3, reach: float) -> Vector3:
	var best := Vector3.INF
	for l: Dictionary in all:
		for q: Vector3 in l.points:
			var d := Vector2(q.x - p.x, q.z - p.z).length()
			if d < reach:
				reach = d
				best = q
	return best


## The side of lot `lot` nearest `p` within `reach` metres, by its index; -1 when none.
static func side_near(lot: Dictionary, p: Vector3, reach: float) -> int:
	var best := -1
	var pts: PackedVector3Array = lot.points
	for i in pts.size():
		var a := Vector2(pts[i].x, pts[i].z)
		var b := Vector2(pts[(i + 1) % pts.size()].x, pts[(i + 1) % pts.size()].z)
		var d := Geometry2D.get_closest_point_to_segment(Vector2(p.x, p.z), a, b).distance_to(Vector2(p.x, p.z))
		if d < reach:
			reach = d
			best = i
	return best
