## The player's lots as the Lots tab draws them (query lots, read again after each edit): each lot's id, zone and outline on the
## ground, each one's front marked with an arrow out through it towards the street (lotTool.ts drawLot), and what of them is under
## the pointer: a lot by its id, the corner nearest a point, the side of a lot nearest it.
class_name LotOutlines
extends RefCounted

## The front's arrow (lotTool.ts drawLot): fronts shorter than front_least_m have none; its length the least of arrow_most_m,
## arrow_front_share of the front and arrow_depth_share of the way to the lot's middle (at least arrow_least_m); its tip
## arrow_tip_share of it short of the front, its head arrow_head_share of it long, the barbs spread arrow_barb_share of the head.
static var front_least_m: float = DataTables.game("tools").lots.front_least_m
static var arrow_most_m: float = DataTables.game("tools").lots.arrow_most_m
static var arrow_front_share: float = DataTables.game("tools").lots.arrow_front_share
static var arrow_least_m: float = DataTables.game("tools").lots.arrow_least_m
static var arrow_depth_share: float = DataTables.game("tools").lots.arrow_depth_share
static var arrow_tip_share: float = DataTables.game("tools").lots.arrow_tip_share
static var arrow_head_share: float = DataTables.game("tools").lots.arrow_head_share
static var arrow_barb_share: float = DataTables.game("tools").lots.arrow_barb_share

var all: Array = [] # {id, zone, points}, as the core gives them
var fronts: Array[PackedVector3Array] = [] # each lot's front and its arrow, as lines (made once a read)


## The lots read again from the core.
func read() -> void:
	all = CityStore.query(&"lots")
	fronts.clear()
	for l: Dictionary in all:
		fronts.append_array(_front_marks(l.points))


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


## A lot's front (its first side: the core keeps the front there) and the arrow from inside the lot out through its middle.
static func _front_marks(points: PackedVector3Array) -> Array[PackedVector3Array]:
	var marks: Array[PackedVector3Array] = []
	if points.size() < 2:
		return marks
	var a := points[0]
	var b := points[1]
	marks.push_back(PackedVector3Array([a, b]))
	var along := Vector2(b.x - a.x, b.z - a.z)
	if along.length() < front_least_m:
		return marks
	var mid := (a + b) * 0.5
	var centre := LotProposals.middle(points)
	var inward := Vector2(mid.x - centre.x, mid.z - centre.z) # from the lot's middle out to its front
	var out := along.normalized().orthogonal()
	if out.dot(inward) < 0.0:
		out = -out
	var size := minf(minf(arrow_most_m, along.length() * arrow_front_share), maxf(arrow_least_m, inward.length() * arrow_depth_share))
	var n := Vector3(out.x, 0.0, out.y)
	var across := Vector3(-out.y, 0.0, out.x)
	var tip := mid - n * size * arrow_tip_share
	var head := size * arrow_head_share
	marks.push_back(PackedVector3Array([mid - n * size, tip]))
	marks.push_back(PackedVector3Array([tip, tip - n * head + across * head * arrow_barb_share]))
	marks.push_back(PackedVector3Array([tip, tip - n * head - across * head * arrow_barb_share]))
	return marks
