## The shoot_test's cases of the fallen: somebody shot lies down as a body of their own, which lands and stays where it fell; somebody
## wounded for good who bleeds out stays there, dead; somebody knocked over by a small blast gets up and is not kept.
class_name ShootBodyCases
extends RefCounted

const LIMIT := 40
const HEAD_AIM_M := 1.7
const LIE_AIM_M := 0.15 # a walker knocked down lies on the ground: the next round is aimed at the body, not at where the chest was
const TRUNK_AIM_M := 1.2
const BLEED_TICKS := 2400
const BODY_FRAMES := 1200
const BODY_SETTLE_FRAMES := 30
const RISE_FORCE := 3
const RISE_KPA := 130.0
const RISE_TRIES := 8
const RISE_LOOK_FRAMES := 20

var t: Node # the shoot_test (TestCase): its check, ticks and aim
var _shoot: Callable
var _shoot_level: Callable
var _walker_of: Callable
var _alone: Callable


func _init(test: Node, shoot: Callable, shoot_level: Callable, walker_of: Callable, alone: Callable) -> void:
	t = test
	_shoot = shoot
	_shoot_level = shoot_level
	_walker_of = walker_of
	_alone = alone


func _frames(n: int) -> void:
	for f in n:
		await t.get_tree().process_frame


## Somebody shot lies down as a body of their own, which comes to rest and stays.
func fallen() -> void:
	await _frames(BODY_SETTLE_FRAMES) # the blows before come and go first: their undo starts the effects over
	var walkers: Array = _alone.call(CityStore.query(&"shot_targets", [LIMIT]).walkers)
	t.check(not walkers.is_empty(), "someone for the body shot")
	if walkers.is_empty():
		return
	var kill: Dictionary = _shoot.call(walkers[0].position + Vector3(0, HEAD_AIM_M * walkers[0].scale, 0))
	print("body shot: ", kill)
	for f in BODY_FRAMES:
		if CityStore.query(&"effects_bodies") > 0:
			break
		await t.get_tree().process_frame
	t.check(CityStore.query(&"effects_bodies") > 0, "who fell is a body")
	for f in BODY_FRAMES:
		if CityStore.query(&"effects_bodies_moving") == 0:
			break
		await t.get_tree().process_frame
	t.check(CityStore.query(&"effects_bodies_moving") == 0, "the body lands")
	var kept: int = CityStore.query(&"effects_bodies_kept")
	t.check(kill.get("killed", false) and kept > 0, "the dead stay where they fell, for good till the bodies' cap: %d kept of %d" % [kept, CityStore.query(&"effects_bodies")])
	await _bled(walkers.slice(1))
	await _rises()


## Somebody shot down for good who bleeds to death where they lie: their body stays, a dead one (BodyRenderer::die); frames between the
## steps, so the effects play each event as it comes.
func _bled(walkers: Array) -> void:
	if not t.check(not walkers.is_empty(), "someone to wound gravely"):
		return
	var kept: int = CityStore.query(&"effects_bodies_kept")
	var bodies: int = CityStore.query(&"effects_bodies")
	var hurt: Dictionary = walkers[0]
	for k in 2:
		var w: Dictionary = _walker_of.call(hurt.id)
		if not w.is_empty():
			_shoot_level.call(w.position + Vector3(0, (LIE_AIM_M if w.fallen else TRUNK_AIM_M * w.scale), 0))
		await _frames(BODY_SETTLE_FRAMES)
	var down: Dictionary = _walker_of.call(hurt.id)
	t.check(not down.is_empty() and down.fallen and down.bleed > 0.0 and CityStore.query(&"effects_bodies") > bodies, "wounded for good: down, bleeding, a body (%s)" % [down])
	t.ticks(BLEED_TICKS)
	await _frames(BODY_SETTLE_FRAMES)
	t.check(CityStore.query(&"effects_bodies_kept") > kept, "who bled out stays where they lay, dead (%d kept, %d before)" % [CityStore.query(&"effects_bodies_kept"), kept])


## Somebody knocked over by a small blast gets up: their body goes and they are walking again, and it is not kept as a dead one. Far
## enough off that nobody dies, and a drum goes one time in two: several tries.
func _rises() -> void:
	var off := 1.0
	while CityStore.query(&"blast_probe", [float(RISE_FORCE), off]).pressure_kpa > RISE_KPA:
		off += 0.5
	var risen := 0
	var kept_before: int = CityStore.query(&"effects_bodies_kept")
	var found: Array = _alone.call(CityStore.query(&"shot_targets", [LIMIT]).walkers)
	for who: Dictionary in found.slice(0, RISE_TRIES):
		var before: int = CityStore.query(&"effects_bodies")
		CityStore.dispatch(&"strike", [who.position + Vector3(off, 0, 0), RISE_FORCE])
		var most := before
		for f in BODY_FRAMES:
			most = maxi(most, CityStore.query(&"effects_bodies"))
			if f > RISE_LOOK_FRAMES and most == before or most > before and CityStore.query(&"effects_bodies") <= before:
				break
			await t.get_tree().process_frame
		risen += most - CityStore.query(&"effects_bodies")
		CityStore.dispatch(&"undo")
		if risen > 0:
			break
	print("on their feet again: ", risen)
	t.check(risen > 0, "who fell gets up")
	t.check(CityStore.query(&"effects_bodies_kept") <= kept_before, "who only fell is not kept as a dead body")
