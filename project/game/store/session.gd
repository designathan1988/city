## What a city comes back with beside its map: the camera, the speed, the Simulation panel's levels and the last choices of
## the Landscape brushes (size, strength, kind: what ToolController remembers), in <slot>.cfg next to the save (the save's own
## format stays the core's). Written with every save, read when the city opens; a missing or unreadable file is a city opened
## with the defaults.
class_name Session
extends RefCounted

const SECTION := "session"
const EXT := ".cfg"
const BRUSHES := "brushes"


static func path(slot: String) -> String:
	return SaveSlots.path(slot).get_basename() + EXT


## The Landscape brushes' last choices, by tool name, as the panels offer them.
static func brushes(tools: ToolController) -> Dictionary:
	var out: Dictionary = {}
	for name in BrushDefaults.tools():
		var held := BrushDefaults.kept(name, tools.remembered.get(ToolController.Tool[name], {}))
		if not held.is_empty():
			out[name] = held
	return out


## The brushes' choices of a file back in the tools' memory: a panel opens on them and the tool takes them in hand.
static func remember_brushes(tools: ToolController, held: Variant) -> void:
	if not held is Dictionary:
		return
	for name in BrushDefaults.tools():
		var kept := BrushDefaults.kept(name, held.get(name))
		if not kept.is_empty():
			tools.remembered[ToolController.Tool[name]] = kept


static func write(slot: String, rig: CityCamera, tools: ToolController = null) -> void:
	var s := CityStore.snapshot
	var cfg := ConfigFile.new()
	cfg.set_value(SECTION, "speed", s.get("speed", 1))
	cfg.set_value(SECTION, "traffic", s.get("traffic_level", 1.0))
	cfg.set_value(SECTION, "people", s.get("people_level", 1.0))
	cfg.set_value(SECTION, "demand", s.get("demand", 1.0))
	cfg.set_value(SECTION, "marking_style", s.get("marking_style", 0))
	cfg.set_value(SECTION, "style", s.get("style", ""))
	if rig != null:
		cfg.set_value(SECTION, "camera", {"focus": rig.focus, "yaw": rad_to_deg(rig.yaw), "pitch": rad_to_deg(rig.pitch), "distance": rig.distance})
	if tools != null:
		cfg.set_value(SECTION, BRUSHES, brushes(tools))
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


static func read(slot: String, rig: CityCamera, paused := false, tools: ToolController = null) -> void:
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
	if tools != null:
		remember_brushes(tools, cfg.get_value(SECTION, BRUSHES, {}))
	var pose: Dictionary = cfg.get_value(SECTION, "camera", {})
	if rig != null and not pose.is_empty():
		rig.jump(pose.focus, pose.yaw, pose.pitch, pose.distance)
