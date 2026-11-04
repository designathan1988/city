## Core test of the history and the save file through the store (no rendering needed): every edit kind is undone to
## exactly the state before it and redone to the state after it (road, demolish, zone stroke, relief stroke, upgrade,
## split, roundabout, drawn building); then a city with edits, grown buildings and sculpted ground is saved, a new map
## replaces it, and opening the file brings back the same counts, money, clock, buildings and heights.
## Run: godot --headless --path project res://tests/core/save_undo_test.tscn -- --no-flow
extends Node3D

const SIGNAL_TICKS := 20
const SIGNAL_OFFSET := 7.0
const PANEL_BUTTONS := 2 # the mode and the green wave
const TOWN_MAP := 0
const SEED := 1
const OTHER_SEED := 7
const PROFILE := 0
const UPGRADE_PROFILE := 3
const OUT_M := 260.0
const ZONE_RADIUS := 60.0
const ZONE_RES := 1
const BRUSH_RADIUS := 60.0
const BRUSH_STRENGTH := 0.4
const BRUSH_DT := 0.5
const BRUSH_DABS := 12
const RING_RADIUS := 22.0
const RING_OUT_M := 700.0
const LOT_M := 30.0
const GROW_TICKS := 600
const PARK_FN := 6
const SLOT := "teste_ida_e_volta"
const MONEY_STEP := 0.01
const KEYS := ["segments", "nodes", "buildings", "money", "zoned_sides"]

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x - 1.0, 0, (b.min.z + b.max.z) * 0.5)
	var e := a + Vector3(OUT_M, 0, 0)
	_undo_cases(a, e)
	_signal_cases()
	await _save_round_trip(a, e)
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## A signal: fixed time takes hold on the next step, undo and redo bring the plan back, a wave sets its neighbours.
func _signal_cases() -> void:
	CityStore.step_ticks(SIGNAL_TICKS)
	var at: Vector3 = CityStore.query(&"traffic_probe", [0]).get("busy_crossing", Vector3.ZERO)
	var was: Dictionary = CityStore.query(&"inspect", [at])
	_check(was.get("kind", "") == "junction", "semáforo inspecionado %s" % [was])
	if was.get("kind", "") != "junction":
		return
	var r: Dictionary = CityStore.dispatch(&"set_signal", [was.id, true, was.greens, SIGNAL_OFFSET])
	var fixed_plan: Dictionary = CityStore.query(&"inspect", [at])
	_check(r.ok and fixed_plan.fixed and fixed_plan.offset == SIGNAL_OFFSET, "tempo fixo %s" % [fixed_plan])
	CityStore.step_ticks(SIGNAL_TICKS)
	_check(CityStore.dispatch(&"undo") and not CityStore.query(&"inspect", [at]).fixed, "tempo fixo desfeito")
	_check(CityStore.dispatch(&"redo") and CityStore.query(&"inspect", [at]).fixed, "tempo fixo refeito")
	CityStore.step_ticks(SIGNAL_TICKS)
	_check(CityStore.dispatch(&"undo"), "volta ao adaptativo")
	CityStore.step_ticks(SIGNAL_TICKS)
	_signal_panel(at, was)
	var wave: Dictionary = CityStore.dispatch(&"green_wave", [was.id])
	var after: Dictionary = CityStore.query(&"inspect", [at])
	print("onda verde: ", wave, " ", after)
	_check(wave.ok == after.fixed, "onda verde deixa o semáforo em tempo fixo, ou diz que não há outro %s" % [wave])
	if wave.ok:
		_check(CityStore.dispatch(&"undo") and not CityStore.query(&"inspect", [at]).fixed, "onda verde desfeita num passo")
		CityStore.step_ticks(SIGNAL_TICKS)


## The panel as the player meets it: drawn from what `inspect` reports, its first button taking the signal to fixed time.
func _signal_panel(at: Vector3, info: Dictionary) -> void:
	var col := VBoxContainer.new()
	add_child(col)
	var redrawn := []
	JunctionPanel.fill(col, info, func() -> void: redrawn.append(true))
	var buttons := col.find_children("*", "Button", true, false)
	_check(buttons.size() >= PANEL_BUTTONS, "painel do semáforo com %d botões" % buttons.size())
	(buttons[0] as Button).pressed.emit()
	_check(not redrawn.is_empty() and CityStore.query(&"inspect", [at]).fixed, "botão do painel passa a tempo fixo")
	_check(CityStore.dispatch(&"undo"), "painel desfeito")
	CityStore.step_ticks(SIGNAL_TICKS)
	col.queue_free()


func _undo_cases(a: Vector3, e: Vector3) -> void:
	var mid := (a + e) * 0.5
	_round("via", func() -> Dictionary: return CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, PROFILE]))
	_round("zona (traço)", func() -> Dictionary:
		CityStore.dispatch(&"begin_stroke", ["cmd.place.zone"])
		var r: Dictionary = CityStore.dispatch(&"paint_zone", [mid, ZONE_RADIUS, ZONE_RES])
		CityStore.dispatch(&"paint_zone", [mid + Vector3(ZONE_RADIUS, 0, 0), ZONE_RADIUS, ZONE_RES])
		CityStore.dispatch(&"end_stroke")
		return r)
	_round("melhorar", func() -> Dictionary: return CityStore.dispatch(&"upgrade_road", [mid, UPGRADE_PROFILE, true]))
	_round("dividir", func() -> Dictionary: return CityStore.dispatch(&"split_road", [mid]))
	_round("demolir", func() -> Dictionary: return CityStore.dispatch(&"demolish", [mid]))
	var ring := e + Vector3(RING_OUT_M, 0, 0)
	_round("rotatória", func() -> Dictionary: return CityStore.dispatch(&"roundabout", [ring, RING_RADIUS, false]))
	var lot := mid + Vector3(0, 0, LOT_M)
	_round("prédio desenhado", func() -> Dictionary:
		return CityStore.dispatch(&"draw_building", [lot, lot + Vector3(LOT_M, 0, LOT_M), PARK_FN, ZONE_RES]))
	var hill := _hill(e)
	var h0 := _height(hill)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.terrainBrush"])
	for i in BRUSH_DABS:
		CityStore.dispatch(&"terrain_brush", [hill, BRUSH_RADIUS, BRUSH_STRENGTH, BRUSH_DT, 0])
	CityStore.dispatch(&"end_stroke")
	var h1 := _height(hill)
	_check(h1 > h0 + 1.0, "relevo subiu %.2f -> %.2f" % [h0, h1])
	CityStore.dispatch(&"undo")
	_check(is_equal_approx(_height(hill), h0), "desfazer relevo volta a %.2f (%.2f)" % [h0, _height(hill)])
	CityStore.dispatch(&"redo")
	_check(is_equal_approx(_height(hill), h1), "refazer relevo volta a %.2f" % h1)


## Free ground for the relief brush: beside the far end of the new road, away from the roundabout.
func _hill(e: Vector3) -> Vector3:
	return e + Vector3(RING_OUT_M * 0.5, 0, RING_OUT_M * 0.5)


## Runs an edit, checks undo returns to the counts before it and redo to the counts after it.
func _round(what: String, edit: Callable) -> void:
	var before := _counts()
	var r: Dictionary = edit.call()
	var after := _counts()
	_check(r.get("ok", false) and after != before, "%s: %s" % [what, r])
	_check(CityStore.dispatch(&"undo") and _counts() == before, "%s desfeito %s" % [what, _counts()])
	_check(CityStore.dispatch(&"redo") and _counts() == after, "%s refeito %s" % [what, _counts()])


func _save_round_trip(a: Vector3, e: Vector3) -> void:
	CityStore.dispatch(&"paint_zone", [(a + e) * 0.5, ZONE_RADIUS, ZONE_RES])
	CityStore.step_ticks(GROW_TICKS)
	var sculpted := _hill(e)
	var saved := _counts()
	saved.grown = CityStore.snapshot.grown
	saved.minutes = CityStore.snapshot.minutes
	var heights := [_height(sculpted), _height(a), _height((a + e) * 0.5)]
	var probes := _probes()
	var signal_at: Vector3 = CityStore.query(&"traffic_probe", [0]).get("busy_crossing", Vector3.ZERO)
	var plan: Dictionary = CityStore.query(&"inspect", [signal_at])
	if plan.get("kind", "") == "junction":
		CityStore.dispatch(&"set_signal", [plan.id, true, plan.greens, SIGNAL_OFFSET])
		plan = CityStore.query(&"inspect", [signal_at])
	var w := await _save(SLOT)
	_check(w.get("ok", false) and not CityStore.snapshot.unsaved, "salvou %s" % w)
	CityStore.new_map(TOWN_MAP, OTHER_SEED, false)
	_check(_counts() != saved, "mapa novo é outro")
	var r: Dictionary = CityStore.load_game(SLOT)
	var got := _counts()
	got.grown = CityStore.snapshot.grown
	got.minutes = CityStore.snapshot.minutes
	_check(r.ok, "abriu %s" % r)
	_check(got == saved, "mesmos números %s == %s" % [got, saved])
	_check([_height(sculpted), _height(a), _height((a + e) * 0.5)] == heights, "mesmas alturas %s" % [heights])
	_check(_probes() == probes, "mesmos prédios (%d sondados)" % probes.size())
	CityStore.step_ticks(SIGNAL_TICKS)
	_check(CityStore.query(&"inspect", [signal_at]) == plan, "mesmo semáforo %s" % [plan])
	_check(not CityStore.snapshot.unsaved and not CityStore.snapshot.can_undo, "aberto sem trabalho a salvar nem histórico")
	DirAccess.remove_absolute(CityStore.save_path(SLOT))


## Saves on the core's thread like the game does and returns the outcome `saved` carried.
func _save(slot: String) -> Dictionary:
	var outcome := {}
	CityStore.saved.connect(func(_s: String, r: Dictionary) -> void: outcome.merge(r), CONNECT_ONE_SHOT)
	CityStore.save_game(slot)
	while CityStore.save_busy():
		await get_tree().process_frame
	return outcome


func _counts() -> Dictionary:
	var s := CityStore.snapshot
	var out := {}
	for k in KEYS:
		out[k] = s[k]
	out.money = snappedf(s.money, MONEY_STEP)
	return out


func _probes() -> Array:
	var out := []
	for id in CityStore.query(&"list_buildings"):
		out.append(CityStore.query(&"probe_building", [id]))
	return out


func _height(p: Vector3) -> float:
	var hit: Dictionary = CityStore.query(&"raycast", [Vector3(p.x, 2000, p.z), Vector3.DOWN])
	return hit.position.y if hit.get("hit", false) else NAN


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
