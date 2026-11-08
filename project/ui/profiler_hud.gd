## Shift+F9: the frame in numbers, redrawn a few times a second from the engine's monitors and the core's render_cost probe; hidden
## it reads nothing. Where the time goes: each step of the last sync in ms; memory and what was drawn.
class_name ProfilerHud
extends PanelContainer


var _text: Label
var _since := 0.0


func _ready() -> void:
	theme_type_variation = &"Popover"
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	minimum_size_changed.connect(_place)
	_text = HudWidgets.label(self, &"MonoLabel")
	hide()


func toggle() -> void:
	visible = not visible
	if visible:
		_refresh()


## By the bottom right corner at the size of its text, again whenever the text changes it.
func _place() -> void:
	reset_size()
	position = get_viewport_rect().size - size - Vector2.ONE * HudWidgets.px("margin")


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
		Performance.get_monitor(Performance.TIME_PROCESS) * Units.MS_PER_S,
		Performance.get_monitor(Performance.RENDER_TOTAL_DRAW_CALLS_IN_FRAME),
		Formatters.format_int(int(Performance.get_monitor(Performance.RENDER_TOTAL_PRIMITIVES_IN_FRAME)))]]
	for k: String in CityStore.constants.frame_sections:
		lines.append("  %-10s %6.2f ms" % [k, float(cost.get(k, 0.0))])
	lines.append("video %d MiB · texture %d MiB · static %d MiB" % [Performance.get_monitor(Performance.RENDER_VIDEO_MEM_USED) / Units.BYTES_PER_MIB,
		Performance.get_monitor(Performance.RENDER_TEXTURE_MEM_USED) / Units.BYTES_PER_MIB, Performance.get_monitor(Performance.MEMORY_STATIC) / Units.BYTES_PER_MIB])
	lines.append(tr("profiler.hide"))
	_text.text = "\n".join(lines)
	reset_size()

