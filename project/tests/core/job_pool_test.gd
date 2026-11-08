## The job pool's rule: a key is a lane and an id, so two users' jobs for the same chunk both run, and a key's newest job replaces a queued one.
extends TestCase


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
	finish()
