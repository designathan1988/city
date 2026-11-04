## The two loops that watch the GPU's frame against its budget. QualityGovernor moves the automatic tier: the p90 of a
## window of frames, down after a few missed windows, up after more roomy ones, never above the tier the hardware was
## given, and a tier the machine could not hold is not tried again for a while. DynamicResolution steps the 3D scale so the
## frame fits. All numbers: data/game/settings.json "governor" and "dynamic_resolution".
class_name FrameGovernor
extends RefCounted


## The cost at `percentile` of a full window.
static func _at_percentile(costs: PackedFloat32Array, percentile: float) -> float:
	var sorted := costs.duplicate()
	sorted.sort()
	return sorted[int(floor(sorted.size() * percentile))]


class Quality:
	extends RefCounted
	var cfg: Dictionary
	var levels: Array
	var level: int
	var ceiling: int
	var costs := PackedFloat32Array()
	var skip := 0
	var missed := 0
	var roomy := 0
	var clock := 0.0
	var last_raise := -INF
	var hold_until := 0.0
	var hold := 0.0

	func _init(config: Dictionary, tier_names: Array, start: String, top: String) -> void:
		cfg = config
		levels = tier_names
		level = levels.find(start)
		ceiling = levels.find(top)
		skip = int(cfg.settle)
		hold = cfg.hold_s

	## The tier changed (by this governor or the player): the windows start again once it has settled.
	func set_level(name: String) -> void:
		level = levels.find(name)
		costs.clear()
		missed = 0
		roomy = 0
		skip = int(cfg.settle)

	## One frame: seconds since the last, its cost and the budget in ms. The tier to switch to, or "" to stay.
	func sample(delta_s: float, cost_ms: float, budget_ms: float) -> String:
		clock += delta_s
		if delta_s > cfg.stall_s or cost_ms <= 0.0 or budget_ms <= 0.0:
			return ""
		if skip > 0:
			skip -= 1
			return ""
		costs.append(cost_ms)
		if costs.size() < int(cfg.window):
			return ""
		var p90 := FrameGovernor._at_percentile(costs, cfg.percentile)
		costs.clear()
		if p90 > budget_ms:
			missed += 1
			roomy = 0
		else:
			missed = 0
			roomy = roomy + 1 if p90 < budget_ms * cfg.up_share else 0
		if missed >= int(cfg.down_windows) and level > 0:
			if clock - last_raise < cfg.undone_s:
				hold_until = clock + hold
				hold *= 2.0
			return levels[level - 1]
		if roomy >= int(cfg.up_windows) and level < ceiling and clock >= hold_until:
			last_raise = clock
			return levels[level + 1]
		return ""


class Resolution:
	extends RefCounted
	var cfg: Dictionary
	var steps: Array
	var scale := 1.0
	var costs := PackedFloat32Array()
	var skip := 0

	func _init(config: Dictionary) -> void:
		cfg = config
		steps = cfg.scales

	## One frame's GPU cost against the budget, ms: the new scale when it should change, 0 to stay.
	func sample(cost_ms: float, budget_ms: float) -> float:
		if cost_ms <= 0.0 or budget_ms <= 0.0:
			return 0.0
		if skip > 0:
			skip -= 1
			return 0.0
		costs.append(cost_ms)
		if costs.size() < int(cfg.window):
			return 0.0
		var cost := FrameGovernor._at_percentile(costs, cfg.percentile)
		costs.clear()
		# the cost goes with the pixels, the square of the linear scale
		var wanted := scale * sqrt(budget_ms * cfg.aim / cost)
		var at := steps.find(scale)
		var next := scale
		if wanted < scale - cfg.deadband:
			next = steps.back()
			for s: float in steps:
				if s <= wanted:
					next = s
					break
		elif at > 0 and wanted >= steps[at - 1] + cfg.rise_margin:
			next = steps[at - 1]
		if is_equal_approx(next, scale):
			return 0.0
		scale = next
		skip = int(cfg.settle)
		return next

	## Back to the whole scale (the option switched off, the tier changed).
	func reset() -> void:
		scale = 1.0
		costs.clear()
		skip = 0
