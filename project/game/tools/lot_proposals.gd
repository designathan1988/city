## What the Lots tab reads about the land: the lots the streets propose where the pointer is (cut for `zone`: works are cut deeper and
## wider), a cell at a time, and the size of a ring as the tag writes it.
class_name LotProposals
extends RefCounted

const UNREAD := Vector2i(1 << 30, 0)
const SHOWN_M := 0.1

var _rings: Array = []
var _flat: Array[PackedVector2Array] = [] # each ring on the ground plane, made once a cell is read
var _cell := UNREAD
var zone := 0 # what the proposal is cut for (0: any use but works)


## What stands changed: read them again.
func forget() -> void:
	_cell = UNREAD


## The proposal under `p`, or an empty ring.
func at(p: Vector3) -> PackedVector3Array:
	var cell_m: float = CityStore.constants.chunk_m # proposals are read a cell of the core's at a time
	var cell := Vector2i(floori(p.x / cell_m), floori(p.z / cell_m))
	if cell != _cell:
		_cell = cell
		_rings = CityStore.query(&"lot_proposals", [p, zone])
		_flat.clear()
		for ring: PackedVector3Array in _rings:
			var flat := PackedVector2Array()
			for q in ring:
				flat.push_back(Vector2(q.x, q.z))
			_flat.append(flat)
	for i in _rings.size():
		if Geometry2D.is_point_in_polygon(Vector2(p.x, p.z), _flat[i]):
			return _rings[i]
	return PackedVector3Array()


## "{w} x {d} m": the front's length, and how far the ring reaches back from it.
static func size_of(ring: PackedVector3Array) -> String:
	var a := Vector2(ring[0].x, ring[0].z)
	var along := (Vector2(ring[1].x, ring[1].z) - a)
	var deep := 0.0
	for q in ring:
		deep = maxf(deep, absf(along.normalized().orthogonal().dot(Vector2(q.x, q.z) - a)))
	return TranslationServer.translate("zone.lot.size").format({"w": Formatters.format_measure(snappedf(along.length(), SHOWN_M)), "d": Formatters.format_measure(snappedf(deep, SHOWN_M))})


static func middle(ring: PackedVector3Array) -> Vector3:
	var sum := Vector3.ZERO
	for q in ring:
		sum += q
	return sum / ring.size()
