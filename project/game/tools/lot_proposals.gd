## What the Lots tab reads about the land: the lots the streets propose where the pointer is, a cell at a time, and the size of a ring as the tag writes it.
class_name LotProposals
extends RefCounted

const CELL_M := 256.0 # core/grid.h CHUNK: proposals are read a cell at a time
const NO_CELL := Vector2i(1 << 30, 0)

var _rings: Array = []
var _cell := NO_CELL


## What stands changed: read them again.
func forget() -> void:
	_cell = NO_CELL


## The proposal under `p`, or an empty ring.
func at(p: Vector3) -> PackedVector3Array:
	var cell := Vector2i(floori(p.x / CELL_M), floori(p.z / CELL_M))
	if cell != _cell:
		_cell = cell
		_rings = CityStore.query(&"lot_proposals", [p])
	for ring: PackedVector3Array in _rings:
		var flat := PackedVector2Array()
		for q in ring:
			flat.push_back(Vector2(q.x, q.z))
		if Geometry2D.is_point_in_polygon(Vector2(p.x, p.z), flat):
			return ring
	return PackedVector3Array()


## "{w} x {d} m": the front's length, and how far the ring reaches back from it.
static func size_of(ring: PackedVector3Array) -> String:
	var a := Vector2(ring[0].x, ring[0].z)
	var along := (Vector2(ring[1].x, ring[1].z) - a)
	var deep := 0.0
	for q in ring:
		deep = maxf(deep, absf(along.normalized().orthogonal().dot(Vector2(q.x, q.z) - a)))
	return TranslationServer.translate("zone.lot.size").format({"w": Formatters.format_measure(snappedf(along.length(), 0.1)), "d": Formatters.format_measure(snappedf(deep, 0.1))})


static func middle(ring: PackedVector3Array) -> Vector3:
	var sum := Vector3.ZERO
	for q in ring:
		sum += q
	return sum / ring.size()
