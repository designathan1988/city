## Two tabs of the F7 panel (the 4.0's lab/resources and lab/inspector): who spends the memory and the frame, each with its bar, and what stands under the pointer,
## with the heights of the point. Draws what the core reports; keeps nothing.
class_name LabViews
extends RefCounted

const MIB := 1048576.0
const BAR_FULL_MS := 0.01 # the bars' full length, when nothing took longer
const SECTIONS := ["near_walls", "near_panes", "near_guards", "near_cards", "near_pieces", "mid_walls", "mid_panes", "mid_cards", "far"]
const CORE_SECTIONS := ["agents", "stream", "levels", "fields", "roads", "props", "buildings"]


static func _row(body: Control, name_text: String, value: String, share: float) -> void:
	var row := HudWidgets.hbox(body)
	var name := HudWidgets.label(row, &"MutedLabel", name_text)
	name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	HudWidgets.label(row, &"BodyLabel", value)
	if share >= 0.0:
		var bar := ProgressBar.new()
		bar.show_percentage = false
		bar.value = share * Formatters.PERCENT
		bar.custom_minimum_size = Vector2(HudWidgets.px("lab_bar_w"), HudWidgets.px("lab_bar_h"))
		row.add_child(bar)


## The building levels by surface and the core's frame by section, the biggest first.
static func consumers(body: Control) -> void:
	HudWidgets.label(body, &"TitleLabel", body.tr("lab.consumers.memory"))
	var cost: Dictionary = CityStore.query(&"building_cost")
	var rows := []
	var most := 1.0
	for key: String in SECTIONS:
		if cost.has(key):
			rows.append([key, float(cost[key].bytes), int(cost[key].triangles)])
			most = maxf(most, float(cost[key].bytes))
	rows.sort_custom(func(a: Array, b: Array) -> bool: return a[1] > b[1])
	for r: Array in rows:
		_row(body, body.tr("lab.surface." + String(r[0])), "%.1f MiB · %s" % [r[1] / MIB, Formatters.format_int(r[2])], r[1] / most)
	HudWidgets.label(body, &"TitleLabel", body.tr("lab.consumers.frame"))
	var frame: Dictionary = CityStore.query(&"render_cost")
	var longest := BAR_FULL_MS
	for key: String in CORE_SECTIONS:
		longest = maxf(longest, float(frame.get(key, 0.0)))
	for key: String in CORE_SECTIONS:
		_row(body, body.tr("lab.core." + key), "%.2f ms" % float(frame.get(key, 0.0)), float(frame.get(key, 0.0)) / longest)


static func core_zones() -> Array[String]:
	var out: Array[String] = []
	out.assign(CORE_SECTIONS)
	return out


## What stands under the pointer and the heights of that point.
static func inspector(body: Control) -> void:
	var tools: ToolController = body.get_tree().get_first_node_in_group(&"tools")
	if tools == null:
		return
	var at: Variant = tools.ground_at(tools.pointer) # where it last stood on the map: reading this, it stands on the panel
	if at == null:
		HudWidgets.label(body, &"MetaLabel", body.tr("lab.inspector.none"))
		return
	var p: Vector3 = at
	_row(body, body.tr("lab.inspector.point"), "%.1f, %.1f" % [p.x, p.z], -1.0)
	_row(body, body.tr("lab.inspector.ground"), "%.2f m" % float(CityStore.query(&"height_at", [p.x, p.z])), -1.0)
	_row(body, body.tr("lab.inspector.pointer"), "%.2f m" % p.y, -1.0)
	var info: Dictionary = CityStore.query(&"inspect", [p])
	if info.is_empty():
		return
	for key: String in info:
		var value: Variant = info[key]
		if value is String or value is int or value is float or value is bool:
			_row(body, key, str(value), -1.0)
