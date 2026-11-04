## What a city comes back with beside its map: the camera, the speed and the Simulation panel's levels, in <slot>.cfg next to the
## save (the save's own format stays the core's). Written with every save, read when the city opens; a missing or unreadable
## file is a city opened with the defaults.
class_name Session
extends RefCounted

const SECTION := "session"
const EXT := ".cfg"


static func path(slot: String) -> String:
	return SaveSlots.path(slot).get_basename() + EXT


static func write(slot: String, rig: CityCamera) -> void:
	var s := CityStore.snapshot
	var cfg := ConfigFile.new()
	cfg.set_value(SECTION, "speed", s.get("speed", 1))
	cfg.set_value(SECTION, "traffic", s.get("traffic_level", 1.0))
	cfg.set_value(SECTION, "people", s.get("people_level", 1.0))
	cfg.set_value(SECTION, "demand", s.get("demand", 1.0))
	if rig != null:
		cfg.set_value(SECTION, "camera", {"focus": rig.focus, "yaw": rad_to_deg(rig.yaw), "pitch": rad_to_deg(rig.pitch), "distance": rig.distance})
	cfg.save(path(slot))


static func read(slot: String, rig: CityCamera) -> void:
	var cfg := ConfigFile.new()
	if cfg.load(path(slot)) != OK:
		return
	CityStore.set_levels(cfg.get_value(SECTION, "traffic", 1.0), cfg.get_value(SECTION, "people", 1.0), cfg.get_value(SECTION, "demand", 1.0))
	CityStore.dispatch(&"set_speed", [cfg.get_value(SECTION, "speed", 1)])
	var pose: Dictionary = cfg.get_value(SECTION, "camera", {})
	if rig != null and not pose.is_empty():
		rig.jump(pose.focus, pose.yaw, pose.pitch, pose.distance)
