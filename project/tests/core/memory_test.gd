## The core's allocator gives back what its threads free: a thread that took memory and freed most of it holds what is left alive
## and no more, and once it freed everything and ended the allocator holds what it held before, with no heap of the thread's own
## left behind. Read on the allocator's own count of what it has committed and of its threads (memory_churn), so the test does not
## depend on what else the process holds.
extends Node

const CHURN_MIB := 256 # what the thread takes: several of the allocator's 32 MiB segments, a block a MiB
const KEEP_ONE_IN := 16 # the blocks it keeps while it frees the others: live blocks beside freed memory, in every segment
const LEFT_MIB := 32.0 # the most the allocator may hold past what lives: one segment (its unit of the system's memory)


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		get_tree().quit(1)
		return
	var seen: Dictionary = CityStore.query(&"memory_churn", [CHURN_MIB, KEEP_ONE_IN])
	print("churn: ", seen)
	var kept := float(CHURN_MIB) / KEEP_ONE_IN
	var failures := 0
	failures += _check(float(seen.most) - float(seen.before) >= CHURN_MIB,
		"the thread held its %d MiB while it used them (%.0f MiB more)" % [CHURN_MIB, float(seen.most) - float(seen.before)])
	failures += _check(float(seen.thin) - float(seen.before) <= kept + LEFT_MIB,
		"freed beside %.0f MiB still alive, the rest went back to the system: %.0f MiB held" % [kept, float(seen.thin) - float(seen.before)])
	failures += _check(float(seen.after) - float(seen.before) <= LEFT_MIB,
		"all freed, it went back to the system: %.0f MiB still held of %d" % [float(seen.after) - float(seen.before), CHURN_MIB])
	failures += _check(int(seen.threads_after) == int(seen.threads_before),
		"the thread that ended left no heap behind: %d threads before, %d after" % [seen.threads_before, seen.threads_after])
	print("FAILURES: ", failures)
	get_tree().quit(0 if failures == 0 else 1)


func _check(good: bool, what: String) -> int:
	print(("ok   " if good else "FAIL ") + what)
	return 0 if good else 1
