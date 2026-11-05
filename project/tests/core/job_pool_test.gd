## The job pool's rule: a key is a lane and an id, so two users' jobs for the same chunk both run, and a key's newest job replaces a queued one.
extends Node


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		get_tree().quit(1)
		return
	var seen: Dictionary = CityStore.query(&"job_pool_probe")
	print("fila: ", seen)
	var good: bool = seen.older == 0 and seen.newer == 1 and seen.other_lane == 1 and seen.replaced == 1
	print("FALHAS: ", 0 if good else 1)
	get_tree().quit(0 if good else 1)
