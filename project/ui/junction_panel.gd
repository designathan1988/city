## A signal in the inspector: adaptive or fixed time, buses first, each stage's green, the offset and the green wave along the main road.
## It keeps nothing: every press goes to the store and the panel is drawn again from what the core reports.
class_name JunctionPanel
extends RefCounted


## `info` is what `inspect` reports of the junction; `redraw` draws the panel again after a change.
static func fill(col: Control, info: Dictionary, redraw: Callable) -> void:
	var step: float = DataTables.ui("hud").signal_step_s
	HudWidgets.label(col, &"TitleLabel", col.tr("junction.signal"))
	var mode := HudWidgets.button(&"SegButton", "", col.tr("junction.mode.fixed" if info.fixed else "junction.mode.adaptive"), "junction.signalMode", "", "")
	mode.pressed.connect(_send.bind(info, not info.fixed, info.greens, info.offset, redraw))
	col.add_child(mode)
	# buses first: a bus near its line holds its green a little, a bus waiting calls its stage (the core's signal_timing.cpp)
	var buses := HudWidgets.button(&"Ghost", "", col.tr("junction.busPriority"), "junction.busPriority", "", "opt_row_h")
	buses.toggle_mode = true
	buses.set_pressed_no_signal(info.buses_first)
	buses.pressed.connect(func() -> void:
		CityStore.dispatch(&"set_signal_plan", [info.id, info.fixed, info.greens, info.offset, not info.buses_first])
		redraw.call())
	col.add_child(buses)
	if info.fixed:
		var greens: PackedFloat32Array = info.greens
		for i in greens.size():
			var label := col.tr("junction.stage").format({"n": i + 1})
			_stepper(col, "%s: %d s" % [label, roundi(greens[i])], _with(greens, i, -step), _with(greens, i, step), info, redraw)
		_offset(col, info, step, redraw)
	var wave := HudWidgets.button(&"SegButton", "", col.tr("junction.greenWave"), "junction.greenWave", "", "")
	wave.pressed.connect(func() -> void:
		CityStore.dispatch(&"green_wave", [info.id])
		redraw.call())
	col.add_child(wave)


static func _offset(col: Control, info: Dictionary, step: float, redraw: Callable) -> void:
	var row := HudWidgets.hbox(col)
	HudWidgets.label(row, &"BodyLabel", "%s: %d s" % [col.tr("junction.offset"), roundi(info.offset)])
	for d: float in [-step, step]:
		var b := HudWidgets.button(&"SegButton", "", "+" if d > 0 else "-", "junction.offset", "", "")
		b.pressed.connect(_send.bind(info, true, info.greens, maxf(0.0, info.offset + d), redraw))
		row.add_child(b)


static func _stepper(col: Control, text: String, less: PackedFloat32Array, more: PackedFloat32Array, info: Dictionary, redraw: Callable) -> void:
	var row := HudWidgets.hbox(col)
	HudWidgets.label(row, &"BodyLabel", text)
	for greens: PackedFloat32Array in [less, more]:
		var b := HudWidgets.button(&"SegButton", "", "-" if greens == less else "+", "junction.stage", "", "")
		b.pressed.connect(_send.bind(info, true, greens, info.offset, redraw))
		row.add_child(b)


static func _with(greens: PackedFloat32Array, i: int, by: float) -> PackedFloat32Array:
	var out := greens.duplicate()
	out[i] = maxf(0.0, out[i] + by)
	return out


static func _send(info: Dictionary, fixed: bool, greens: PackedFloat32Array, offset: float, redraw: Callable) -> void:
	CityStore.dispatch(&"set_signal_plan", [info.id, fixed, greens, offset, info.buses_first])
	redraw.call()
