## What the road tool keeps between sessions: the snaps, the keys changed and the cut's walls.
class_name RoadPrefs
extends RefCounted

const SECTION := "road_snap"

var snap := {"on": true, "angles": true, "grid": true}
var keys := {}
var walls := false


func read(cfg: ConfigFile) -> void:
	for key in snap:
		snap[key] = cfg.get_value(SECTION, key, snap[key])
	keys = cfg.get_value(SECTION, "keys", {})
	walls = cfg.get_value(SECTION, "walls", false)


func write(cfg: ConfigFile) -> void:
	for key in snap:
		cfg.set_value(SECTION, key, snap[key])
	cfg.set_value(SECTION, "keys", keys)
	cfg.set_value(SECTION, "walls", walls)
