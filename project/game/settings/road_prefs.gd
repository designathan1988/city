## What the road tool keeps between sessions: the snaps, the keys changed, the cut's walls and how new roads are lit.
class_name RoadPrefs
extends RefCounted

const SECTION := "road_snap"

var snap := {"on": true, "angles": true, "grid": true}
var keys := {}
var walls := false
## New roads lit by the power line's poles, their lamps on them (Roadcraft roadTool.ts layPart): else by lighting columns.
var power := true
## The grid the next click lays, armed by the option (Roadcraft toolChoices blockGridChoice): for this session.
var blocks := {"armed": false, "cols": 0, "rows": 0, "size": 0.0, "turn": 0.0}


func read(cfg: ConfigFile) -> void:
	for key in snap:
		snap[key] = cfg.get_value(SECTION, key, snap[key])
	keys = cfg.get_value(SECTION, "keys", {})
	walls = cfg.get_value(SECTION, "walls", false)
	power = cfg.get_value(SECTION, "power", true)


func write(cfg: ConfigFile) -> void:
	for key in snap:
		cfg.set_value(SECTION, key, snap[key])
	cfg.set_value(SECTION, "keys", keys)
	cfg.set_value(SECTION, "walls", walls)
	cfg.set_value(SECTION, "power", power)
