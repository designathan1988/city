## The lots of the lot lab set down on open ground, one picture of each in a row, the whole of it from the street's side: --only=lotes (--only=lotes,church for one).
class_name CaptureLots
extends RefCounted

const OFF_M := 900.0
const ROAD_M := 400.0
const PX := 0.25
const VIEW_UP := 0.55
const VIEW_OUT := 1.1


static func shots(cap: Node) -> void:
	Settings.apply(cap.world, cap.get_viewport())
	UiDriver.at_hour(cap.world, 22.0 if cap.only.has("noite") else 16.0)
	var a := Vector3(OFF_M, 0, -ROAD_M * 0.5)
	CityStore.dispatch(&"build_road", [a, a + Vector3(0, 0, ROAD_M), false, Vector3.ZERO, CityStore.first_road(), PX])
	var mid := a + Vector3(0, 0, ROAD_M * 0.5)
	for e: Dictionary in LotProjects.entries():
		var name: String = e.key.trim_prefix(LotProjects.KEY_PREFIX)
		if cap.only.size() > 1 and not cap.only.has(name):
			continue
		var size: Vector3 = CityStore.query(&"body_size", [e.bytes])
		var fit: Dictionary = CityStore.query(&"place_fit", [mid, Vector2(size.x, size.y), 0.0])
		var r: Dictionary = CityStore.dispatch(&"place_model", [e.bytes, fit.x, fit.z, fit.rot, e.function, e.zone])
		print(name, " ", size, " ", r.get("error", ""))
		if not r.get("ok", false):
			continue
		var at: Vector3 = cap._ground(fit.x, fit.z)
		var inward := Vector3(sin(fit.rot), 0, cos(fit.rot)) * -1.0
		var reach: float = maxf(maxf(size.x, size.y), size.z)
		var centre := at + inward * size.y * 0.5 + Vector3.UP * size.z * 0.4
		await cap._shot("lote_" + name, centre - inward * reach * VIEW_OUT + Vector3.UP * reach * VIEW_UP, centre)
		CityStore.dispatch(&"undo")
