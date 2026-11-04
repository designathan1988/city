## Shift+F9: the frame in numbers, redrawn a few times a second from the engine's monitors and the core's render_cost probe; hidden
## it reads nothing. Where the time goes: each step of the last sync in ms; memory and what was drawn.
class_name ProfilerHud
extends PanelContainer

const MIB := 1048576.0
const SYNC_PARTS := ["lock", "stream", "levels", "flush", "roads", "props", "buildings", "trees", "agents"]

@onready var _text: Label
@onready var _since := 0.0


func _ready() -> void:
	theme_type_variation = &"Popover"
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	set_anchors_and_offsets_preset(Control.PRESET_BOTTOM_RIGHT)
	grow_horizontal = Control.GROW_DIRECTION_BEGIN
	grow_vertical = Control.GROW_DIRECTION_BEGIN
	offset_right = -HudWidgets.px("margin")
	offset_bottom = -(HudWidgets.px("compass") + HudWidgets.px("compass_margin") * 2.0)
	_text = HudWidgets.label(self, &"MetaLabel")
	hide()


func toggle() -> void:
	visible = not visible
	if visible:
		_refresh()


func _process(delta: float) -> void:
	if not visible:
		return
	_since += delta
	if _since >= DataTables.game("render_switches").stats_every_s:
		_since = 0.0
		_refresh()


func _refresh() -> void:
	var cost: Dictionary = CityStore.query(&"render_cost")
	var lines := ["PROFILER  %d fps · %.1f ms · %d calls · %s tris" % [Performance.get_monitor(Performance.TIME_FPS),
		Performance.get_monitor(Performance.TIME_PROCESS) * ProfilerHud.ms_per_s(),
		Performance.get_monitor(Performance.RENDER_TOTAL_DRAW_CALLS_IN_FRAME),
		Formatters.format_int(int(Performance.get_monitor(Performance.RENDER_TOTAL_PRIMITIVES_IN_FRAME)))]]
	for k: String in SYNC_PARTS:
		lines.append("  %-10s %6.2f ms" % [k, float(cost.get(k, 0.0))])
	lines.append("video %d MiB · texture %d MiB · static %d MiB" % [Performance.get_monitor(Performance.RENDER_VIDEO_MEM_USED) / MIB,
		Performance.get_monitor(Performance.RENDER_TEXTURE_MEM_USED) / MIB, Performance.get_monitor(Performance.MEMORY_STATIC) / MIB])
	lines.append(tr("profiler.hide"))
	_text.text = "\n".join(lines)
	reset_size()


static func ms_per_s() -> float:
	return HealthMs.MS_PER_S


## The unit the monitors report seconds in.
class HealthMs:
	const MS_PER_S := 1000.0
