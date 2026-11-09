## What a city comes back with beside its map: the camera, the speed and the Simulation panel's levels, and whether the city is the
## player's (`kept`: edited or saved by them; CitySaves lets go of one that is not when another city takes its place), in <slot>.cfg
## next to the save (the save's own format stays the core's). Written with every save, read when the city opens; a missing or
## unreadable file is a city opened with the defaults, and the player's.
class_name Session
extends RefCounted

const SECTION := "session"
const EXT := ".cfg"
const KEPT := "kept"
const KEPT_UNMARKED := true # a city with no `kept` mark (saved before it) is the player's: never let go unasked


static func path(slot: String) -> String:
	return SaveSlots.path(slot).get_basename() + EXT


## `kept`: the city is the player's (CitySaves).
static func write(slot: String, rig: CityCamera, kept: bool) -> void:
	var s := CityStore.snapshot
	var cfg := ConfigFile.new()
	cfg.set_value(SECTION, KEPT, kept)
	cfg.set_value(SECTION, "speed", s.get("speed", 1))
	cfg.set_value(SECTION, "traffic", s.get("traffic_level", 1.0))
	cfg.set_value(SECTION, "people", s.get("people_level", 1.0))
	cfg.set_value(SECTION, "demand", s.get("demand", 1.0))
	cfg.set_value(SECTION, "marking_style", s.get("marking_style", 0))
	cfg.set_value(SECTION, "style", s.get("style", ""))
	if rig != null:
		cfg.set_value(SECTION, "camera", {"focus": rig.focus, "yaw": rad_to_deg(rig.yaw), "pitch": rad_to_deg(rig.pitch), "distance": rig.distance})
	cfg.save(path(slot))


## What a file holds, brought back to what the panels offer (a file edited by hand, or from another version): the demand and the speed
## to the nearest offered, the levels into their range; what is no number takes the default.
static func normalized(traffic: Variant, people: Variant, demand: Variant, speed: Variant) -> Dictionary:
	var sim: Dictionary = DataTables.ui("drawers").popovers.sim
	var top: float = float(sim.level_max_pct) / Units.PERCENT
	var demands: Array = sim.demand.map(func(d: Array) -> float: return d[0])
	return {"traffic": clampf(_number(traffic, 1.0), 0.0, top), "people": clampf(_number(people, 1.0), 0.0, top),
		"demand": _nearest(_number(demand, 1.0), demands),
		"speed": clampi(int(_number(speed, 1.0)), CityStore.constants.speed_paused, CityStore.constants.speed_fastest)}


static func _number(v: Variant, fallback: float) -> float:
	return float(v) if (v is float or v is int) and is_finite(float(v)) else fallback


static func _nearest(v: float, among: Array) -> float:
	var best: float = among[0]
	for c: float in among:
		if absf(c - v) < absf(best - v):
			best = c
	return best


## Whether the saved city is the player's. A city written before the mark existed, or whose session file is gone, carries no mark:
## it counts as the player's (KEPT_UNMARKED), since letting go of a city is only for one known never to have been theirs.
static func kept(slot: String) -> bool:
	var cfg := ConfigFile.new()
	if cfg.load(path(slot)) != OK: # no session beside the save: an unmarked city
		return KEPT_UNMARKED
	return bool(cfg.get_value(SECTION, KEPT, KEPT_UNMARKED))


static func read(slot: String, rig: CityCamera, paused := false) -> void:
	var cfg := ConfigFile.new()
	if cfg.load(path(slot)) != OK:
		return
	var n := normalized(cfg.get_value(SECTION, "traffic", 1.0), cfg.get_value(SECTION, "people", 1.0), cfg.get_value(SECTION, "demand", 1.0),
			cfg.get_value(SECTION, "speed", 1))
	CityStore.set_levels(n.traffic, n.people, n.demand)
	if not paused:
		CityStore.dispatch(&"set_speed", [n.speed])
	CityStore.dispatch(&"set_marking_style", [int(cfg.get_value(SECTION, "marking_style", 0)), false])
	CityStore.dispatch(&"set_style", [str(cfg.get_value(SECTION, "style", ""))])
	var pose: Dictionary = cfg.get_value(SECTION, "camera", {})
	if rig != null and not pose.is_empty():
		rig.jump(pose.focus, pose.yaw, pose.pitch, pose.distance)
