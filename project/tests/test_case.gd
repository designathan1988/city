## TestCase: the one harness of the core and end-to-end tests. One check (an `ok` or `FAIL` line each), one ending
## (`FAILURES: n` and the exit code the orchestrator reads, tools/tasks/verdicts.py), a test that checked nothing fails (it
## proved nothing), and one clock rule: every map opens paused and moves only by the ticks the test steps, so no tick of the
## simulation thread lands between what a test sets and what it reads.
class_name TestCase
extends Node

## The tests' own files (a model written, a damaged save, a report): a folder of their own, never the player's user://.
const SCRATCH := "user://tests"
## The most frames a save on the core's thread may take before the test calls it stuck.
const SAVE_FRAMES := 3000

var failures := 0
var checks := 0
var world: CityWorld


## The world the store draws into, and the store booted on the data tables: false, and a failure, when the boot refuses.
func boot() -> bool:
	world = CityWorld.new()
	add_child(world)
	var errors := CityStore.boot(world.terrain, world.materials())
	return check(errors.is_empty(), "boot %s" % [errors])


## One check, printed; a failure is counted. Returns `ok`, so a test can stop where a failure makes the rest meaningless.
func check(ok: bool, what: String) -> bool:
	checks += 1
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
	return ok


## A map of data/catalog/maps.json opened paused (CityStore.new_map's `paused`): it moves only by `ticks`.
func new_map(map: int, seed: int, with_city: bool) -> Dictionary:
	return CityStore.new_map(map, seed, with_city, true)


## The simulation `count` ticks on, on this thread. The core refuses while its clock runs (Simulation::hold_still): that is a
## failure of the test, which opened its map running or set a speed.
func ticks(count: int) -> void:
	if not CityStore.step_ticks(count):
		check(false, "%d ticks asked while the clock runs: open the map paused" % count)


## The city written to `slot` (the tests' saves folder, SaveSlots), or with "" to its own slot as the Menu's Save does, and the
## outcome. A save on the core's thread is waited for until its `saved` comes; a refused one returns at once. Past SAVE_FRAMES
## the wait is a failure, not a hang.
func save(slot := "") -> Dictionary:
	var outcome := {}
	var keep := func(saved_slot: String, result: Dictionary) -> void:
		if slot.is_empty() or saved_slot == slot:
			outcome.merge(result)
	CityStore.saved.connect(keep)
	var started: Dictionary = CityStore.saves.save_city() if slot.is_empty() else CityStore.saves.save_game(slot)
	var waited := 0
	while outcome.is_empty() and started.get("pending", false) and waited < SAVE_FRAMES:
		await get_tree().process_frame
		waited += 1
	CityStore.saved.disconnect(keep)
	if outcome.is_empty() and started.get("pending", false):
		check(false, "the save of %s never ended (%d frames)" % [slot, waited])
	return outcome if not outcome.is_empty() else started


## The path of one of the tests' own files, its folder made first.
func scratch(file: String) -> String:
	DirAccess.make_dir_recursive_absolute(SCRATCH)
	return "%s/%s" % [SCRATCH, file]


## The end of every test: the count of failures and the exit code, 0 only when something was checked and nothing failed.
func finish() -> void:
	if checks == 0:
		check(false, "the test made no check")
	print("FAILURES: %d" % failures)
	get_tree().quit(1 if failures > 0 else 0)
