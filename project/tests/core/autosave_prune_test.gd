## Core test of the autosave of a city the player never touched (the lab's main.ts snapshotApplied writes the map on screen, touched
## or not, so the next start opens it) and of letting it go when another city takes its place (the lab's next map is written over
## it): a new map is written by the autosave without becoming the player's, and its file goes when the next map opens; a city the
## player saved stays; a file the autosave wrote, opened again and replaced untouched, goes too; and the same city opened again
## replaces nothing.
## Run: godot --headless --path project res://tests/core/autosave_prune_test.tscn -- --no-flow
extends TestCase

const MAP := 0 # empty maps of the town's size: no city to grow, the test is about the files
const SEEDS := [11, 12, 13, 14]


func _ready() -> void:
	if not boot():
		finish()
		return
	CityStore.saves.after_edit = true # the write after a new map, as the game runs it (tests keep it off by default)
	var first := await _untouched_city(SEEDS[0])
	check(not first.is_empty() and SaveSlots.exists(first), "a new map nobody touched is written by the autosave (%s)" % first)
	check(not Session.kept(first), "the autosave does not make it the player's")
	var second := await _untouched_city(SEEDS[1])
	check(not SaveSlots.exists(first) and not FileAccess.file_exists(Session.path(first)),
			"the untouched city's file and session go when another city opens")
	var saved: Dictionary = await save()
	check(saved.get("ok", false) and Session.kept(second), "Save makes the city the player's: %s" % saved)
	var third := await _untouched_city(SEEDS[2])
	check(SaveSlots.exists(second), "the player's city stays when another opens")
	var again: Dictionary = await CityStore.saves.load_game(third, true)
	check(again.get("ok", false) and SaveSlots.exists(third), "the same city opened again replaces nothing")
	check(CityStore.saves.city_slot() == third, "it keeps its slot")
	await _untouched_city(SEEDS[3])
	check(not SaveSlots.exists(third), "an untouched city opened from its file goes when another takes its place")
	var opened: Dictionary = await CityStore.saves.load_game(second, true)
	check(opened.get("ok", false) and SaveSlots.exists(second), "the player's city opens from its file")
	new_map(MAP, SEEDS[0], false)
	check(SaveSlots.exists(second), "and stays when another city takes its place")
	CityStore.saves.after_edit = false
	finish()


## A new empty map, left alone until the autosave has written it: its slot ("" when nothing was written).
func _untouched_city(seed: int) -> String:
	new_map(MAP, seed, false)
	CityStore.refresh()
	CityStore.saves.tick(0.0) # the new map's revision seen
	CityStore.saves.tick(DataTables.game("saves").autosave_quiet_s)
	var waited := 0
	while CityStore.saves.save_busy() and waited < SAVE_FRAMES:
		CityStore.saves.tick(0.0)
		await get_tree().process_frame
		waited += 1
	check(not CityStore.saves.save_busy(), "the autosave of seed %d ended" % seed)
	return CityStore.saves.city_slot()
