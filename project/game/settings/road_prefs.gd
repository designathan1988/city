## What the road tool keeps between sessions: the snaps, the keys changed and the cut's walls.
class_name RoadPrefs
extends RefCounted

const SECTION := "road_snap"

var snap := {"on": true, "angles": true, "grid": true}
var keys := {}
var walls := false
## The grid the next click lays, armed by the option (Roadcraft toolChoices blockGridChoice): for this session.
var blocks := {"armed": false, "cols": 0, "rows": 0, "size": 0.0, "turn": 0.0}


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
