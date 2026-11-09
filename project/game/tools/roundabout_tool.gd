## Roundabout (its own tab): a click on vacant ground lays a ring of eight nodes with four approaches (the lab's
## editor/roundabout.ts); near a road it is refused. [ and ] scale the radius; the disc previews it green or red with the
## core's refusal.
extends ToolBase

## [ and ] scale by this (data/game/tools.json roundabout).
static var scale_step: float = DataTables.game("tools").roundabout.scale_step

var radius := 0.0


func enter() -> void:
	radius = params.get("radius", ctl.last_param(ToolController.Tool.ROUNDABOUT, "radius", _spec().default))
	ctl.hint.emit("", true)


func _spec() -> Dictionary:
	for c in DataTables.ui("drawers").categories:
		if c.has("ring"):
			return c.ring
	return {}


func exit() -> void:
	ctl.preview.clear()


func hover(p: Vector3) -> void:
	var pv: Dictionary = CityStore.query(&"preview_roundabout", [p, radius])
	if pv.is_empty():
		return
	ctl.preview.show_disc(pv.centre, radius, pv.ok)
	ctl.hint.emit("" if pv.ok else tr(pv.error), pv.ok)


func click(p: Vector3) -> void:
	var r: Dictionary = CityStore.dispatch(&"roundabout", [p, radius])
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)
		return
	ctl.preview.clear()
	ctl.hint.emit("%s · %s" % [tr("ui.do.roundabout"), Formatters.format_cost(r.money)], true)


func key(keycode: Key) -> void:
	var lo: float = DataTables.tuning("roundabout_radius_min_m")
	var hi: float = DataTables.tuning("roundabout_radius_max_m")
	if keycode == KEY_BRACKETRIGHT:
		radius = clampf(radius * scale_step, lo, hi)
	elif keycode == KEY_BRACKETLEFT:
		radius = clampf(radius / scale_step, lo, hi)
	ctl.remembered[ToolController.Tool.ROUNDABOUT] = {"radius": radius}
