## Close looks at the bus, the truck, the motorbike, the van and a sedan nearest to the centre for the visual capture
## (traffic_probe sample, sample_dir and sample_type): their glass, the cabin and the people inside, from ahead, from the
## kerb side low at the windows and from above; the van from ahead and behind, the sedan from behind (their ends).
class_name CaptureVehicles
extends RefCounted

## sim VehicleType (native/src/sim/catalog/traffic_params.h)
const SEDAN := 0
const VAN := 3
const MOTO := 5
const BUS := 6
const TRUCK := 7
const CAR_VIEW_FROM := Vector3(3, 2.5, 14) # x to its left, y up, z ahead, as in VIEWS
const CAR_HIGH_FROM := Vector3(0, 45, 50)
const CAR_VIEW_AT := Vector3(0, 0.5, 0)
const CAMERA_CLEAR_M := 1.5
## Per type: name, camera offset in the vehicle frame (x to its left, y up, z ahead) and the height looked at.
const VIEWS := {
	BUS: [["22_onibus", Vector3(4, 3, 16), 1.6], ["22_onibus_lado", Vector3(-7, 2.6, 1), 1.6], ["22_onibus_alto", Vector3(7, 9, 4), 1.0]],
	TRUCK: [["23_caminhao", Vector3(3, 3, 11), 1.8], ["23_caminhao_lado", Vector3(-5, 2.8, 4), 2.0], ["23_caminhao_alto", Vector3(5, 8, 3), 1.5]],
	MOTO: [["24_moto", Vector3(2, 1.6, 5), 1.0]],
	VAN: [["25_van", Vector3(3, 2.4, 8), 1.1], ["25_van_tras", Vector3(3, 2.4, -8), 1.1]],
	SEDAN: [["26_sedan_tras", Vector3(0.8, 2.4, -7.5), 0.7]],
}


## The car nearest to `ground` whose low view has room: a vehicle or a building at the camera's point fills the picture.
static func car_shots(cap: Node, probe: Dictionary, ground: Vector3) -> void:
	var order: Array = range(probe.sample.size())
	order.sort_custom(func(a: int, b: int) -> bool: return (probe.sample[a] as Vector3).distance_to(ground) < (probe.sample[b] as Vector3).distance_to(ground))
	for k: int in order:
		var eye := _eye(probe, k, CAR_VIEW_FROM)
		if int(probe.sample_type[k]) >= MOTO or not CityStore.query(&"camera_clear", [eye, CAMERA_CLEAR_M]):
			continue
		var car: Vector3 = probe.sample[k]
		await cap._shot("07_carro", eye, car + CAR_VIEW_AT)
		await cap._shot("08_carro_alto", _eye(probe, k, CAR_HIGH_FROM), car)
		return
	print("captura: nenhum carro com a câmera fora de geometria")


## `cap` is the capture scene (its _shot).
static func shots(cap: Node, probe: Dictionary, ground: Vector3) -> void:
	for type in VIEWS:
		var k := _nearest_of(probe, type, ground)
		if k < 0:
			print("captura: nenhum veículo do tipo ", type, " na amostra")
			continue
		var at: Vector3 = probe.sample[k]
		for v in VIEWS[type]:
			await cap._shot(v[0], _eye(probe, k, v[1]), at + Vector3.UP * v[2])
			print("custo dos veículos em ", v[0], ": ", CityStore.query(&"agent_cost"))


## Where the camera stands for sample `k`, `off` in the vehicle's frame.
static func _eye(probe: Dictionary, k: int, off: Vector3) -> Vector3:
	var ahead: Vector3 = probe.sample_dir[k]
	var left := Vector3(ahead.z, 0, -ahead.x)
	return (probe.sample[k] as Vector3) + left * off.x + Vector3.UP * off.y + ahead * off.z


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
