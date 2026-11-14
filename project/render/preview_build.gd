## The road in hand as it will be built (Roadcraft main.ts drawBuildModes, drawTunnelStretches), drawn into the tool
## preview's mesh from the core's plan: `deck` (each station at its solved height), `ground` (the natural ground under
## it), `ways` (how it is built there), `piers` (foot and head of each) and the two ends' authored heights. An embankment
## and a cutting are skirts from the road's two edges to the ground, a cutting's rim a line on the ground, a bored stretch
## dimmed, a pier a post under the deck, and an end drawn off the ground a dashed post down to it. Colours and widths are
## the preview tokens of data/ui/tokens.json; the materials are the preview's own (PreviewLine).
class_name PreviewBuild
extends RefCounted

## The core's BuildWay (sim/economy/road_economy.h).
enum Way { GROUND, EMBANKMENT, TRENCH, BRIDGE, TUNNEL }

const HALF := 0.5


## mats: the materials of `fill`, `cut`, `rim`, `buried`, `pier` and `post`.
static func draw(mesh: ImmediateMesh, plan: Dictionary, mats: Dictionary) -> void:
	var deck: PackedVector3Array = plan.get("deck", PackedVector3Array())
	if deck.size() < 2:
		return
	var lift := Vector3.UP * ThemeBuilder.size("preview_lift")
	_earth(mesh, plan, deck, lift, mats)
	_uprights(mesh, plan, deck, lift, mats)


## The ground point under an end of the road in hand (its last end, or its first): where its ring is drawn. A plan with no
## stations (refused before it was solved) gives the end itself.
static func foot(plan: Dictionary, last: bool) -> Vector3:
	var deck: PackedVector3Array = plan.get("deck", PackedVector3Array())
	if deck.is_empty():
		var points: PackedVector3Array = plan.points
		return points[-1] if last else points[0]
	var i := deck.size() - 1 if last else 0
	return Vector3(deck[i].x, plan.ground[i], deck[i].z)


## The skirts of what is filled or cut, a cutting's rim on the ground, and the bored stretch dimmed over the road.
static func _earth(mesh: ImmediateMesh, plan: Dictionary, deck: PackedVector3Array, lift: Vector3, mats: Dictionary) -> void:
	var ground: PackedFloat32Array = plan.ground
	var ways: PackedByteArray = plan.ways
	var half: float = plan.half_width
	var rim_m := ThemeBuilder.size("preview_rim_m")
	var fill := PackedVector3Array()
	var cut := PackedVector3Array()
	var rim := PackedVector3Array()
	var buried := PackedVector3Array()
	for i in deck.size() - 1:
		var a := deck[i] + lift
		var b := deck[i + 1] + lift
		var along := Vector3(b.x - a.x, 0, b.z - a.z)
		if ways[i] == Way.GROUND or ways[i] == Way.BRIDGE or along.is_zero_approx():
			continue
		var left := Vector3(along.z, 0, -along.x).normalized()
		if ways[i] == Way.TUNNEL:
			_quad(buried, a + left * half, b + left * half, b - left * half, a - left * half)
			continue
		var ga := Vector3(a.x, ground[i] + lift.y, a.z)
		var gb := Vector3(b.x, ground[i + 1] + lift.y, b.z)
		for side: float in [-1.0, 1.0]:
			var out := left * half * side
			_quad(fill if ways[i] == Way.EMBANKMENT else cut, a + out, b + out, gb + out, ga + out)
			if ways[i] == Way.TRENCH:
				_quad(rim, ga + out, gb + out, gb + out + left * rim_m * side, ga + out + left * rim_m * side)
	_emit(mesh, fill, mats.fill)
	_emit(mesh, cut, mats.cut)
	_emit(mesh, rim, mats.rim)
	_emit(mesh, buried, mats.buried)


## The piers under what stands on them, and at each end drawn off the ground a dashed post between it and the ground.
static func _uprights(mesh: ImmediateMesh, plan: Dictionary, deck: PackedVector3Array, lift: Vector3, mats: Dictionary) -> void:
	var piers: PackedVector3Array = plan.piers
	var posts := PackedVector3Array()
	for k in range(0, piers.size() - 1, 2):
		_post(posts, piers[k] + lift, piers[k + 1] + lift, ThemeBuilder.size("preview_pier_m") * HALF)
	_emit(mesh, posts, mats.pier)
	var dashes := PackedVector3Array()
	var dash := ThemeBuilder.size("preview_post_dash_m")
	for last: bool in [false, true]:
		if is_zero_approx(plan.lift_b if last else plan.lift_a):
			continue
		var head: Vector3 = deck[deck.size() - 1 if last else 0] + lift
		var base := foot(plan, last) + lift
		var count := maxi(1, int(absf(head.y - base.y) / dash))
		for k in range(0, count, 2): # one dash drawn, one left out
			_post(dashes, base.lerp(head, float(k) / count), base.lerp(head, float(k + 1) / count), ThemeBuilder.size("preview_post_m") * HALF)
	_emit(mesh, dashes, mats.post)


## An upright seen from any side: two crossed faces between `base` and `head`.
static func _post(out: PackedVector3Array, base: Vector3, head: Vector3, half: float) -> void:
	for axis: Vector3 in [Vector3.RIGHT, Vector3.BACK]:
		_quad(out, base - axis * half, base + axis * half, head + axis * half, head - axis * half)


static func _quad(out: PackedVector3Array, a: Vector3, b: Vector3, c: Vector3, d: Vector3) -> void:
	out.append_array(PackedVector3Array([a, b, c, a, c, d]))


## One surface of triangles; none when there is nothing to draw (a surface with no vertex is an error).
static func _emit(mesh: ImmediateMesh, triangles: PackedVector3Array, material: Material) -> void:
	if triangles.is_empty():
		return
	mesh.surface_begin(Mesh.PRIMITIVE_TRIANGLES, material)
	for v in triangles:
		mesh.surface_add_vertex(v)
	mesh.surface_end()
