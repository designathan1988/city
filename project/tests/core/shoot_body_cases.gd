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
const UNDER_GROUND_M := 0.02 # a limb's middle under the surface by more than this: in the ground (a limb stands 6 cm thick over it)
const CONTACT_SLACK_M := 0.02 # two limbs nearer than they touch by more than this: one sunk in the other (each stands 6 cm from its middle)
const SIDE_BLOW_M := 4.0 # how far to one side of two walkers together a blow throws both the same way
const TOGETHER_FORCE := 3

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
	var marks_before: int = CityStore.query(&"damage_probe").blood_marks
	var kill: Dictionary = _shoot.call(walkers[0].position + Vector3(0, HEAD_AIM_M * walkers[0].scale, 0))
	print("body shot: ", kill)
	for f in BODY_FRAMES:
		if _census().bodies > 0:
			break
		await t.get_tree().process_frame
	t.check(_census().bodies > 0, "who fell is a body")
	for f in BODY_FRAMES:
		if _census().moving == 0:
			break
		await t.get_tree().process_frame
	t.check(_census().moving == 0, "the body lands")
	await _frames(BODY_SETTLE_FRAMES) # the frame it falls asleep lays its pool
	var marks: int = CityStore.query(&"damage_probe").blood_marks
	t.check(marks > marks_before, "a pool spreads under the body where it came to rest (blood marks %d -> %d)" % [marks_before, marks])
	var kept: int = _census().kept
	t.check(kill.get("killed", false) and kept > 0, "the dead stay where they fell, for good till the bodies' cap: %d kept of %d" % [kept, _census().bodies])
	await _bled(walkers.slice(1))
	var deep: float = _census().deepest
	t.check(deep <= UNDER_GROUND_M, "no body at rest lies in the ground, its bones held out as its joints are (deepest %.3f m)" % deep)
	await _rises()
	await _together()


func _census() -> Dictionary:
	return CityStore.query(&"effects_bodies_census")


## Two people walking side by side, thrown the same way by a blow to one side of them (between them it would throw them apart): their
## bodies lie on one another, no joint of one sunk in the other (BodyRenderer census overlap; Ragdoll::push_out_of).
func _together() -> void:
	var standing: Array = []
	for w: Dictionary in CityStore.query(&"shot_targets", [LIMIT * 4]).walkers:
		if not w.fallen:
			standing.push_back(w.position)
	var pair := LookAim.nearest_pair(standing, Vector2.ZERO, INF) # anywhere in the town
	if not t.check(not pair.is_empty(), "two people walking together"):
		return
	var before: int = _census().bodies
	CityStore.dispatch(&"strike", [pair[0] + Vector3(-SIDE_BLOW_M, 0, 0), TOGETHER_FORCE])
	for f in BODY_FRAMES:
		if _census().bodies > before and _census().moving == 0:
			break
		await t.get_tree().process_frame
	await _frames(BODY_SETTLE_FRAMES)
	var census := _census()
	print("thrown together: ", census)
	t.check(census.bodies >= before + 2, "both of them thrown down (%d bodies, %d before)" % [census.bodies, before])
	t.check(census.overlap <= CONTACT_SLACK_M, "bodies at rest lie on one another, none sunk in another (deepest %.3f m in)" % census.overlap)


## Somebody shot down for good who bleeds to death where they lie: their body stays, a dead one (BodyRenderer::die); frames between the
## steps, so the effects play each event as it comes.
func _bled(walkers: Array) -> void:
	if not t.check(not walkers.is_empty(), "someone to wound gravely"):
		return
	var kept: int = _census().kept
	var bodies: int = _census().bodies
	var pieces: int = _census().pieces
	var hurt: Dictionary = walkers[0]
	for k in 2:
		var w: Dictionary = _walker_of.call(hurt.id)
		if not w.is_empty():
			_shoot_level.call(w.position + Vector3(0, (LIE_AIM_M if w.fallen else TRUNK_AIM_M * w.scale), 0))
		await _frames(BODY_SETTLE_FRAMES)
	var down: Dictionary = _walker_of.call(hurt.id)
	t.check(not down.is_empty() and down.fallen and down.bleed > 0.0 and _census().bodies > bodies, "wounded for good: down, bleeding, a body (%s)" % [down])
	# the second round, in a limb of somebody lying, takes it off (shoot_people.cpp): a piece of the same body on a ragdoll of its own
	if int(down.get("lost", 0)) != 0:
		t.check(_census().pieces > pieces, "the limb shot off is a piece of the body (%d pieces, %d before)" % [_census().pieces, pieces])
	t.ticks(BLEED_TICKS)
	await _frames(BODY_SETTLE_FRAMES)
	t.check(_census().kept > kept, "who bled out stays where they lay, dead (%d kept, %d before)" % [_census().kept, kept])


## Somebody knocked over by a small blast gets up: their body goes and they are walking again, and it is not kept as a dead one. Far
## enough off that nobody dies, and a drum goes one time in two: several tries.
func _rises() -> void:
	var off := 1.0
	while CityStore.query(&"blast_probe", [float(RISE_FORCE), off]).pressure_kpa > RISE_KPA:
		off += 0.5
	var risen := 0
	var kept_before: int = _census().kept
	var found: Array = _alone.call(CityStore.query(&"shot_targets", [LIMIT]).walkers)
	for who: Dictionary in found.slice(0, RISE_TRIES):
		var before: int = _census().bodies
		CityStore.dispatch(&"strike", [who.position + Vector3(off, 0, 0), RISE_FORCE])
		var most := before
		for f in BODY_FRAMES:
			most = maxi(most, _census().bodies)
			if f > RISE_LOOK_FRAMES and most == before or most > before and _census().bodies <= before:
				break
			await t.get_tree().process_frame
		risen += most - _census().bodies
		CityStore.dispatch(&"undo")
		if risen > 0:
			break
	print("on their feet again: ", risen, " (kept ", _census().kept, ", before ", kept_before, ": a death beside the blow is kept)")
	# a kept body never leaves: those that rose and left were not kept (who died beside the blow is, rightly)
	t.check(risen > 0, "who fell gets up, and the body goes: not kept as a dead one")
