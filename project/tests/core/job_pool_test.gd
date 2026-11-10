## The job pool's rule: a key is a lane and an id, so two users' jobs for the same chunk both run, and a key's newest job replaces a queued one.
## And its memory: jobs that come one after the other stay on one worker, and what they built, freed by their caller afterwards, is
## the system's again once the pool rests.
extends Node

const LEFT_MIB := 32.0 # the most the allocator may still hold of the serial jobs' memory: one of its segments


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		get_tree().quit(1)
		return
	var seen: Dictionary = CityStore.query(&"job_pool_probe")
	print("fila: ", seen)
	var failures := 0
	failures += _check(seen.older == 0 and seen.newer == 1 and seen.other_lane == 1 and seen.replaced == 1 and seen.keys_left == 0,
		"a key's newest job replaces a queued one, another lane's job for the same chunk runs, no key is kept")
	failures += _check(int(seen.serial_threads) == 1, "jobs that come one after the other stay on one worker: %d used" % seen.serial_threads)
	failures += _check(float(seen.serial_left_mib) <= LEFT_MIB,
		"what the jobs built and their caller freed went back to the system: %.0f MiB still held" % seen.serial_left_mib)
	failures += _check(float(seen.late_left_mib) <= LEFT_MIB,
		"memory freed long after its job, with the worker at rest, went back to the system too: %.0f MiB still held" % seen.late_left_mib)
	print("FAILURES: ", failures)
	get_tree().quit(0 if failures == 0 else 1)


func _check(good: bool, what: String) -> int:
	print(("ok   " if good else "FAIL ") + what)
	return 0 if good else 1
