## The zone brush as tests and captures lay it, the way the player paints it: strokes over the land the streets' lots stand on
## (paint_zone_path), one command. `along` runs down each side of a road; `around` crosses a point of a road whose way is unknown.
class_name ZoneStroke
extends RefCounted

## How far from the road's line each stroke runs: past any profile's footway, inside the lots behind it (the street's lots reach
## 32 m back from the footway, the works' 50 m).
const SIDE_M := 25.0
## The stroke's step: shorter than any lot's front.
const STEP_M := 2.0


## Down both sides of the road from a to b.
static func along(a: Vector3, b: Vector3, zone: int, side_m := SIDE_M) -> Dictionary:
	var run := Vector3(b.x - a.x, 0, b.z - a.z)
	var left := Vector3(run.z, 0, -run.x).normalized() * side_m
	var steps := maxi(1, ceili(run.length() / STEP_M))
	var dabs := PackedVector3Array()
	for off: Vector3 in [left, -left]:
		for k in steps + 1:
			dabs.append(a.lerp(b, float(k) / steps) + off)
	return CityStore.dispatch(&"paint_zone_path", [dabs, zone])


## Four short strokes round a point of a road, one each way: whichever way the road runs, two of them cross the land beside it.
static func around(p: Vector3, zone: int, side_m := SIDE_M) -> Dictionary:
	var dabs := PackedVector3Array()
	for off: Vector3 in [Vector3.RIGHT, Vector3.LEFT, Vector3.FORWARD, Vector3.BACK]:
		dabs.append(p + off * side_m)
	return CityStore.dispatch(&"paint_zone_path", [dabs, zone])
