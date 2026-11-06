## The falls of the demolition captures: a tall building, and people knocked over who get up (--phase=torre,derrubados).
class_name CaptureFalls
extends RefCounted

const TOWER_FLOORS := 8
const TOWER_FLOORS_MOST := 40
const TOWER_FORCE := 16
const TOWER_DIST := 320.0
const KNOCK_VIEW_M := 7.0
const KNOCK_KPA := 200.0
const KNOCK_FORCE := 3
const KNOCK_SHOTS := 9
const KNOCK_EVERY_S := 0.6
const WALKER_PITCH := 25.0
const TARGETS := 80


## How the walls of a tall building break and how the break runs up it, seen from a street away.
static func tower(blast: CaptureBlast, tall: Array) -> void:
	if not tall.is_empty():
		await blast.blow("torre", tall[tall.size() - 1], TOWER_FORCE, TOWER_DIST, [0.2, 0.6, 1.2, 2.0, 3.5, 9.0])


## A small blast in a crowd: who is knocked over lies a moment and gets up where they stood.
static func knocked(rig: CityCamera, ui: UiDriver, host: Node) -> void:
	var found: Array = CityStore.query(&"shot_targets", [TARGETS]).walkers
	if found.is_empty():
		return
	var at: Vector3 = found[0].position
	rig.jump(at, CaptureBlast.VIEW_YAW, WALKER_PITCH, KNOCK_VIEW_M)
	await ui.drawn()
	var off := 1.0
	while CityStore.query(&"blast_probe", [float(KNOCK_FORCE), off]).pressure_kpa > KNOCK_KPA: # where the drums go and nobody dies
		off += 0.5
	CityStore.dispatch(&"strike", [at + Vector3(off, 0, 0), KNOCK_FORCE])
	for k in KNOCK_SHOTS:
		await host.get_tree().create_timer(KNOCK_EVERY_S).timeout
		await ui.shot("derrubado_%02d" % k)
