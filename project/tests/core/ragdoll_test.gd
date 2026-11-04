## Core test of the people that fall (data/render/ragdoll.json, native/src/sim/physics, render/effects/ragdoll_world): the solver's own
## measures on every cooked variation dropped and thrown over flat ground (the joints stay together, no limit of the NASA table is
## passed by more than a degree, no skin point goes into the ground, the body comes to rest), and the game's own path: a walker shot
## dead falls as a body, one that comes to rest costs nothing more.
## Run: godot --path project res://tests/core/ragdoll_test.tscn -- --no-flow  (the cooked people need the renderer's meshes)
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const WARM_TICKS := 600
const VARIATIONS := 32
const DROP_M := 1.0
const THROW_FORWARD_MS := 6.3 # the blow's push on a body: effects.json down.push_ms (9) times the heavy body's share (0.7)
const THROW_UP_MS := 2.6
const FALL_S := 8.0
const GAP_M := 0.01 # a joint's two pivots stay together to a centimetre
const OVERSHOOT_DEG := 1.0 # no limit of the table is passed by more than a degree
const SKIN_IN_GROUND_M := 0.02
const LYING_TOP_M := 1.0 # at rest nobody stands: the highest skin point is under this
const ALONE_M := 6.0
const TARGETS := 80
const HEAD_AIM_M := 1.7
const STANDOFF := Vector3(0.0, 6.0, 12.0)
const SETTLE_FRAMES := 6
const FREE_DROP_M := 50.0
const FREE_FALL_S := 0.5
const VIEW_YAW := 0.0
const VIEW_PITCH := 25.0
const VIEW_DIST := 8.0
const SUBSTEP_SWEEP := []
var failures := 0
var rig: CityCamera


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	Settings.apply(world, get_viewport())
	rig = CityCamera.new() # the frame the renderer syncs in needs a camera, as the game
	add_child(rig)
	var errors := CityStore.boot(world.terrain, world.materials())
	_check(errors.is_empty(), "boot %s" % [errors])
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	CityStore.step_ticks(WARM_TICKS)
	await get_tree().process_frame
	_free_fall()
	_solver()
	await _game_path()
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## The measures of the whole cooked file at `substeps` (0: the data's own): the worst of each, and which joint it was.
func _sweep(substeps: int) -> Dictionary:
	var worst := {"gap_m": 0.0, "overshoot_deg": 0.0, "penetration_m": 0.0, "top_m": 0.0, "rest_s": 0.0, "us_avg": 0.0, "us_max": 0.0, "unrested": 0}
	var joint_over := []
	var joint_gap := []
	for v in VARIATIONS:
		for throw in [false, true]:
			var m: Dictionary = CityStore.query(&"ragdoll_probe", [v, THROW_FORWARD_MS if throw else 0.0, THROW_UP_MS if throw else 0.0, DROP_M, FALL_S, substeps])
			if m.has("error"):
				_check(false, "variação %d: %s" % [v, m.error])
				continue
			for k in ["gap_m", "overshoot_deg", "penetration_m", "top_m", "rest_s", "us_avg", "us_max"]:
				worst[k] = maxf(worst[k], m[k])
			if m.rest_s < 0.0:
				worst.unrested += 1
			if joint_over.is_empty():
				joint_over.resize(m.joint_overshoot_deg.size())
				joint_over.fill(0.0)
				joint_gap.resize(m.joint_gap_m.size())
				joint_gap.fill(0.0)
			for j in m.joint_overshoot_deg.size():
				joint_over[j] = maxf(joint_over[j], m.joint_overshoot_deg[j])
				joint_gap[j] = maxf(joint_gap[j], m.joint_gap_m[j])
	worst["joint_over_deg"] = joint_over.map(func(x): return snappedf(x, 0.1))
	worst["joint_gap_cm"] = joint_gap.map(func(x): return snappedf(x * 100.0, 0.1))
	return worst


## In free fall nothing touches anything: every joint must stay as it was built, so what shows here is what the bind pose itself asks
## of the limits (a bind pose outside a limit is the data's fault, not the solver's).
func _free_fall() -> void:
	for v in [0, 1, 17]:
		var m: Dictionary = CityStore.query(&"ragdoll_probe", [v, 0.0, 0.0, FREE_DROP_M, FREE_FALL_S, 0])
		print("queda livre variação %d: sobra dos limites (graus) %s, folga das juntas (cm) %s, componente %s" % [v,
				m.joint_overshoot_deg.map(func(x): return snappedf(x, 0.1)), m.joint_gap_m.map(func(x): return snappedf(x * 100.0, 0.01)), m.joint_part])


func _solver() -> void:
	for substeps in SUBSTEP_SWEEP:
		print("substeps %d: %s" % [substeps, _sweep(substeps)])
	var worst := _sweep(0)
	print("dado: %s" % [worst])
	_check(worst.gap_m < GAP_M, "as juntas ficam juntas: %.4f m" % worst.gap_m)
	_check(worst.overshoot_deg < OVERSHOOT_DEG, "nenhum limite passado: %.2f graus" % worst.overshoot_deg)
	_check(worst.penetration_m < SKIN_IN_GROUND_M, "nenhum ponto da pele no chão: %.4f m" % worst.penetration_m)
	_check(worst.top_m < LYING_TOP_M, "em repouso ninguém está de pé: %.2f m" % worst.top_m)
	_check(worst.unrested == 0, "todos chegam ao repouso em %.0f s: %d não" % [FALL_S, worst.unrested])


func _game_path() -> void:
	var walkers: Array = CityStore.query(&"shot_targets", [TARGETS]).walkers
	_check(walkers.size() > 0, "pedestres para o tiro")
	if walkers.is_empty():
		return
	var w: Dictionary = walkers[0]
	rig.jump(w.position, VIEW_YAW, VIEW_PITCH, VIEW_DIST) # the renderer draws what the camera is near
	for k in SETTLE_FRAMES:
		await get_tree().process_frame
	_check(int(CityStore.query(&"fallen_people")) == 0, "ninguém caído antes")
	var origin: Vector3 = w.position + Vector3(0, HEAD_AIM_M, 0) + STANDOFF
	var r: Dictionary = CityStore.dispatch(&"shoot", [origin, (w.position + Vector3(0, HEAD_AIM_M * w.scale, 0) - origin).normalized()])
	_check(r.get("hit", 0) == 1, "o tiro acertou %s" % [r])
	for k in SETTLE_FRAMES:
		await get_tree().process_frame
	_check(int(CityStore.query(&"fallen_people")) >= 1, "o pedestre morto cai como corpo: %s" % [CityStore.query(&"ragdoll_cost")])


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
