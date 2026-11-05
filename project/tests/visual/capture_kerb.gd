## The dropped kerb of the busy crossing for the visual capture (traffic_probe crossing_kerb: [kerb point, along the
## arm, across]): seen low along the kerb and from the carriageway in front.
class_name CaptureKerb
extends RefCounted

## Offsets in the kerb's frame: x across toward the footway, y up, z along the arm away from the junction.
const ALONG_FROM := Vector3(-1.2, 1.0, 9)
const ALONG_AT := Vector3(0, 0.1, -1.5)
const FRONT_FROM := Vector3(-5, 1.6, 0)
const FRONT_AT := Vector3(1, 0, 0)


## `cap` is the capture scene (its _shot); `kerb` the probe's frame, empty when the city has no lit crossing.
static func shots(cap: Node, kerb: Array) -> void:
	if kerb.is_empty():
		return
	var at: Vector3 = kerb[0]
	var along: Vector3 = kerb[1]
	var across: Vector3 = kerb[2]
	var place := func(o: Vector3) -> Vector3: return at + across * o.x + Vector3.UP * o.y + along * o.z
	await cap._shot("20_rebaixo", place.call(ALONG_FROM), place.call(ALONG_AT))
	await cap._shot("20_rebaixo_frente", place.call(FRONT_FROM), place.call(FRONT_AT))
