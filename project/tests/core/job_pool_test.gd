## The job pool's rule: a key is a lane and an id, so two users' jobs for the same chunk both run, and a key's newest job replaces a queued one.
## And its memory: jobs that come one after the other stay on one worker, and what they built, freed by their caller afterwards, is
## the system's again once the pool rests.
extends TestCase

const LEFT_MIB := 32.0 # the most the allocator may still hold of the serial jobs' memory: one of its segments


func _ready() -> void:
	if not boot():
		finish()
		return
	var seen: Dictionary = CityStore.query(&"job_pool_probe")
	print("queue: ", seen)
	check(seen.older == 0, "the queued job its key's newer one replaced never ran: %d" % seen.older)
	check(seen.newer == 1, "the newer job of the key ran once: %d" % seen.newer)
	check(seen.other_lane == 1, "the other lane's job for the same chunk ran: %d" % seen.other_lane)
	check(seen.replaced == 1, "one job replaced: %d" % seen.replaced)
	check(seen.keys_left == 0, "no key kept once its job ran: %d" % seen.keys_left)
	check(int(seen.serial_threads) == 1, "jobs that come one after the other stay on one worker: %d used" % seen.serial_threads)
	check(float(seen.serial_left_mib) <= LEFT_MIB,
		"what the jobs built and their caller freed went back to the system: %.0f MiB still held" % seen.serial_left_mib)
	check(float(seen.late_left_mib) <= LEFT_MIB,
		"memory freed long after its job, with the worker at rest, went back to the system too: %.0f MiB still held" % seen.late_left_mib)
	finish()
