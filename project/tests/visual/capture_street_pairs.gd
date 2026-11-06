## The pairs with the Roadcraft 4.0 (capture_street.gd, `cap`): each pose its capture recorded, the eye and the look in the frame
## of a thing (along the street, out from the road, up; metres) and the lens, put here on the same kind of thing. Given by
## --poses=file (JSON, name -> pose), saved as par_<name>.png at the hours of the 4.0's pictures. The plates stand one at a time
## on a stretch of footway with nothing before them, as they did in the 4.0.
class_name CaptureStreetPairs
extends RefCounted

const NIGHT := 22.0
const NEAR_M := 0.05
const CLEAR_STEP_M := 1.0
const CLEAR_TRIES := 200
const CLEAR_BEFORE_M := 6 # of footway free in front of the plate, where the eye looks along
const KEYS := {
	"banco": "streetscape.bench", "lixeira": "streetscape.bin", "hidrante": "streetscape.hydrant", "luminaria": "streetscape.lamp",
	"cerca": "barrier.kind.fence", "muro": "barrier.kind.wall", "sebe": "barrier.kind.hedge", "defensa": "barrier.kind.guardrail",
	"gradil": "barrier.kind.railing", "sol-bench": "streetscape.bench", "sol-bin": "streetscape.bin", "sol-hydrant": "streetscape.hydrant",
	"sol-lamp": "streetscape.lamp",
}
const ON_THE_RUN := ["banco", "lixeira", "hidrante", "luminaria", "poste", "sol-bench", "sol-bin", "sol-hydrant", "sol-lamp", "sol-pole", "sol-rua"] # on the run's footway, the road at +side; the rest across it


## The picture the game shows (game/play.gd): without it a wire of half a pixel breaks up on the photo.
static func picture(world: CityWorld, viewport: Viewport) -> void:
	Settings.apply(world, viewport)
	print("qualidade: ", Settings.preset, " msaa ", viewport.msaa_3d, " escala ", viewport.scaling_3d_scale)


static func run(cap: Node, a: Vector3, dir: Vector3) -> void:
	var path := ""
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--poses="):
			path = arg.trim_prefix("--poses=")
	if path.is_empty():
		return
	var poses: Dictionary = JSON.parse_string(FileAccess.get_file_as_string(path))
	var side := dir.cross(Vector3.UP)
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	var camera: Camera3D = cap.rig.camera
	var lens := camera.fov
	var plate_at := _clear_spot(cap, dir)
	for night: bool in [false, true]:
		UiDriver.at_hour(cap.world, NIGHT if night else cap.DAY)
		await cap.ui.frames(int(ProjectSettings.get_setting("rendering/reflections/sky_reflections/roughness_layers")) + 1)
		for name: String in poses:
			if name.ends_with("noite") != night:
				continue
			if not night:
				UiDriver.at_hour(cap.world, poses[name].get("hour", cap.DAY))
			var thing := name.trim_suffix("-noite").trim_suffix("-perto")
			if thing in ["semaforo", "esquina"]:
				var arm: Array = probe.crossing_kerb
				await _shot(cap, name, poses[name], probe.busy_crossing, arm[1], arm[2])
				continue
			if thing.begins_with("placa"):
				await _plate(cap, name, poses[name], int(thing.get_slice("-", 1)), plate_at, -dir, side)
				continue
			var at: Vector3 = a if thing in ["poste", "sol-pole", "sol-rua"] else cap.spots.get(KEYS.get(thing, ""), Vector3.INF)
			if at == Vector3.INF:
				print("a pair with no object: ", name)
				continue
			var out := -side if thing in ON_THE_RUN else side
			await _shot(cap, name, poses[name], at, _handed(dir, out, poses[name].hand), out)
	camera.fov = lens


## Where a plate can stand on the footway across the run with nothing on the way to the eye, looking along `dir`.
static func _clear_spot(cap: Node, dir: Vector3) -> Vector3:
	for k in CLEAR_TRIES:
		var at: Dictionary = CityStore.query(&"preview_item", [cap._across - dir * k * CLEAR_STEP_M, cap.ITEM_SIGN, 1.0])
		if not at.get("ok", false):
			continue
		var free := true
		for d in range(1, CLEAR_BEFORE_M + 1):
			free = free and CityStore.query(&"preview_item", [at.position - dir * d, cap.ITEM_SIGN, 1.0]).get("ok", false)
		if free:
			return at.position
	return Vector3.INF


static func _plate(cap: Node, name: String, pose: Dictionary, type: int, at: Vector3, along: Vector3, out: Vector3) -> void:
	var made: Dictionary = CityStore.dispatch(&"place_item", [at, cap.ITEM_SIGN, type, cap.SIGN_WORDS, 1.0])
	cap._check(made.get("ok", false), "pair: sign %d standing" % type)
	await _shot(cap, name, pose, at, along, out)
	CityStore.dispatch(&"remove_street", [at, 1.0])


## `along` or its opposite, for (along, out) to turn the way they did in the 4.0's map (its y runs against this world's z).
static func _handed(along: Vector3, out: Vector3, hand: float) -> Vector3:
	return along if signf(along.x * -out.z + along.z * out.x) == signf(hand) else -along


static func _shot(cap: Node, name: String, p: Dictionary, at: Vector3, along: Vector3, out: Vector3) -> void:
	var camera: Camera3D = cap.rig.camera
	var ground: Vector3 = cap.ui.ground(at.x, at.z)
	camera.fov = p.fov
	camera.near = NEAR_M
	camera.global_position = ground + along * p.along + out * p.out + Vector3.UP * p.up
	camera.look_at(camera.global_position + along * p.fa + out * p.fo + Vector3.UP * p.fu)
	await cap.ui.drawn()
	await cap.ui.shot("par_" + name)
