## Who is on foot, through the census of the core: out of the doors and in at them, of each age, in pairs that stay together,
## no child alone, fewer in the small hours than in the afternoon, and nobody inside a street piece or another walker.
## Run: godot --path project res://tests/core/people_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const SETTLE_FRAMES := 2
const RIDER_CLIPS := ["chairSit", "carDrive", "carRide", "carRearRide", "cabDrive", "cabRide", "motoRide", "motoStop", "bikeStop", "bikePedal"]
const WARMUP_TICKS := 4800
const WARMUP_FROM := 12.0
const SMALL_HOURS := 3.0
const MIN_PARTY_SHARE := 0.4 # Moussaid 2010: 55 % and 70 % of the people; the census of the Roadcraft 4.0: 55 %
const MAX_PARTY_SHARE := 0.75
const MIN_ELDERS := 0.08
const MAX_ELDERS := 0.25
const MIN_CHILDREN := 0.05
const MAX_CHILDREN := 0.25
const FAR_PARTY_M := 4.0 # the two of a pair are never further apart than a few steps
const MAX_TOUCHING := 0.01 # a stride's overlap as one turns while the other stands: not more than one pair in a hundred
const CROSS_STEPS := 1500 # a street sees few people an hour: as long as it takes somebody to cross
const CROSS_TICKS := 30
const NEAR_M := 150.0 # the census of the Roadcraft 4.0: 86 people within this of the camera at 16:00
const MORE_TICKS := 9000
const MIN_INDOORS := 0.5 # of those who arrive; the others, on a street without a door, end at the node

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var cam := Camera3D.new()
	add_child(cam)
	cam.look_at_from_position(Vector3(0, 300, 300), Vector3.ZERO)
	cam.current = true
	var errors := CityStore.boot(world.terrain, world.materials())
	_check(errors.is_empty(), "boot %s" % [errors])
	_check(CityStore.new_map(TOWN_MAP, SEED, true).get("ok", false), "mapa gerado")
	CityStore.dispatch(&"set_speed", [0])
	await _frames(SETTLE_FRAMES)
	CityStore.query(&"set_city_hour", [WARMUP_FROM]) # the clock runs: at the census it reads the afternoon's hour
	CityStore.step_ticks(WARMUP_TICKS)
	var c: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, 0.0])
	print("censo: ", c)
	print("perto: ", CityStore.query(&"people_census", [Vector3.ZERO, NEAR_M]).total, " em ", NEAR_M, " m")
	var total := float(c.total)
	_check(total > 0, "gente na rua")
	_check(int(c.doors) > 0, "portas lidas: %d" % c.doors)
	_check(int(c.doors_across) == 0, "caminho da porta cruza a rua: %d" % c.doors_across)
	_check(int(c.came_out) > 0, "gente saiu de porta: %d" % c.came_out)
	_check(int(c.went_in) > 0, "gente entrou em porta: %d" % c.went_in)
	var share := 1.0 - float(c.solo) / total
	_check(share >= MIN_PARTY_SHARE and share <= MAX_PARTY_SHARE, "em grupo: %.2f" % share)
	_check(float(c.elders) / total >= MIN_ELDERS and float(c.elders) / total <= MAX_ELDERS, "idosos: %d" % c.elders)
	_check(float(c.children) / total >= MIN_CHILDREN and float(c.children) / total <= MAX_CHILDREN, "crianças: %d" % c.children)
	_check(int(c.children_alone) == 0, "criança sozinha: %d" % c.children_alone)
	_check(float(c.party_gap_m) <= FAR_PARTY_M, "par longe: %.2f m" % c.party_gap_m)
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	print("corpos: ", probe.people_overlaps, " móveis: ", probe.people_in_furniture)
	_check(int(probe.people_in_furniture) == 0, "dentro de móvel: %d" % probe.people_in_furniture)
	_check(float(c.party_touching) <= MAX_TOUCHING * float(c.parties), "par encostado: %d" % c.party_touching)
	_clips()
	CityStore.step_ticks(MORE_TICKS)
	var later: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, 0.0])
	print("depois: ", later)
	var doing := 0
	for act in later.acts:
		doing += int(later.acts[act])
	_check(doing > 0, "quem espera na guia não faz nada: %s" % later.acts)
	var indoors := float(later.went_in) / maxf(float(later.arrived), 1.0)
	_check(indoors >= MIN_INDOORS, "chegaram por uma porta: %.2f" % indoors)
	_mid_block()
	var afternoon := float(c.wanted)
	CityStore.query(&"set_city_hour", [SMALL_HOURS])
	var night: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, 0.0])
	print("à tarde ", afternoon, " às 3 h ", night.wanted)
	_check(float(night.wanted) < afternoon, "menos gente de madrugada")
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


## The clips of people in vehicles (tools/assets/rider_poses.py): every one in the file, in the block, and a body seated in it.
func _clips() -> void:
	var clips: Array = CityStore.query(&"people_clips")
	var named := {}
	for c: Dictionary in clips:
		named[c.name] = c
	for name: String in RIDER_CLIPS:
		_check(named.has(name), "clipe no arquivo: %s" % name)
		_check(named.has(name) and float(named[name].pelvis.y) > 0.0 and float(named[name].above) > 0.0, "pelve e alcance de %s" % name)
	_check(CityStore.query(&"show_people_clip", ["chairSit"]), "toca um clipe pelo nome")
	_check(not CityStore.query(&"show_people_clip", ["sem nome"]), "recusa clipe que não há")
	CityStore.query(&"show_people_clip", ["idle"])


## People go over a crossing between junctions, a zebra and one with a signal of its own: put where walkers are, and counted while they use it.
func _mid_block() -> void:
	for kind: int in [RoadPanel.ZEBRA, RoadPanel.SIGNAL]:
		var made := false
		var census: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, 0.0])
		for key: String in census.spots:
			var seg: Dictionary = CityStore.query(&"segment_at", [census.spots[key][0]])
			if seg.get("seg", -1) >= 0:
				var points: Array = seg.points
				made = CityStore.dispatch(&"add_crossing", [points[points.size() / 2], kind]).get("ok", false)
			if made:
				break
		_check(made, "faixa no meio da quadra onde há gente, tipo %d" % kind)
		var crossed := 0
		var waited := 0
		for i in CROSS_STEPS:
			CityStore.step_ticks(CROSS_TICKS)
			var now: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, 0.0])
			crossed = maxi(crossed, now.mid_crossing)
			waited = maxi(waited, now.mid_waiting)
			if crossed > 0:
				break
		print("faixa tipo ", kind, ": atravessando ", crossed, " esperando ", waited)
		_check(crossed > 0, "ninguém usou a faixa no meio da quadra, tipo %d (esperando: %d)" % [kind, waited])
		CityStore.dispatch(&"undo")


func _check(ok: bool, what: String) -> void:
	if not ok:
		failures += 1
		print("FALHOU: ", what)
