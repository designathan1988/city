## Close looks at the bus, the truck and the motorbike nearest to the centre for the visual capture (traffic_probe
## sample, sample_dir and sample_type): their glass, the cabin and the people inside, from ahead, from the kerb side
## low at the windows and from above.
class_name CaptureVehicles
extends RefCounted

## sim VehicleType (native/src/sim/catalog/traffic_params.h)
const MOTO := 5
const BUS := 6
const TRUCK := 7
## Per type: name, camera offset in the vehicle frame (x to its left, y up, z ahead) and the height looked at.
const VIEWS := {
	BUS: [["22_onibus", Vector3(4, 3, 16), 1.6], ["22_onibus_lado", Vector3(-7, 2.6, 1), 1.6], ["22_onibus_alto", Vector3(7, 9, 4), 1.0]],
	TRUCK: [["23_caminhao", Vector3(3, 3, 11), 1.8], ["23_caminhao_lado", Vector3(-5, 2.8, 4), 2.0], ["23_caminhao_alto", Vector3(5, 8, 3), 1.5]],
	MOTO: [["24_moto", Vector3(2, 1.6, 5), 1.0]],
}


## `cap` is the capture scene (its _shot).
static func shots(cap: Node, probe: Dictionary, ground: Vector3) -> void:
	for type in VIEWS:
		var k := _nearest_of(probe, type, ground)
		if k < 0:
			print("captura: nenhum veículo do tipo ", type, " na amostra")
			continue
		var at: Vector3 = probe.sample[k]
		var ahead: Vector3 = probe.sample_dir[k]
		var left := Vector3(ahead.z, 0, -ahead.x)
		for v in VIEWS[type]:
			var off: Vector3 = v[1]
			await cap._shot(v[0], at + left * off.x + Vector3.UP * off.y + ahead * off.z, at + Vector3.UP * v[2])


## Index in the sample of the vehicle of `type` nearest to `to`, -1 when there is none.
static func _nearest_of(probe: Dictionary, type: int, to: Vector3) -> int:
	var best := -1
	var best_d := INF
	for i in probe.sample.size():
		var d: float = (probe.sample[i] as Vector3).distance_to(to)
		if int(probe.sample_type[i]) == type and d < best_d:
			best_d = d
			best = i
	return best
