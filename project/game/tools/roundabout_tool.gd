## Roundabout (Roundabout tab): a click on vacant ground lays a ring with four
## approaches; on a junction (or with Shift) the roads inside the circle are cut and join the ring. [ and ] scale the
## radius; the disc previews it green or red with the core's refusal.
extends ToolBase

## [ and ] scale by this.
const SCALE_STEP := 1.2

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
	var pv: Dictionary = CityStore.query(&"preview_roundabout", [p, radius, Input.is_key_pressed(KEY_SHIFT)])
	if pv.is_empty():
		return
	ctl.preview.show_disc(pv.centre, radius, pv.ok)
	ctl.hint.emit("" if pv.ok else tr(pv.error), pv.ok)


func click(p: Vector3) -> void:
	var r: Dictionary = CityStore.dispatch(&"roundabout", [p, radius, Input.is_key_pressed(KEY_SHIFT)])
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)
		return
	ctl.preview.clear()
	ctl.hint.emit(tr("ui.do.roundabout") + " · " + Formatters.format_cost(r.money), true)


func key(keycode: Key) -> void:
	var lo: float = DataTables.tuning("roundabout_radius_min_m")
	var hi: float = DataTables.tuning("roundabout_radius_max_m")
	if keycode == KEY_BRACKETRIGHT:
		radius = clampf(radius * SCALE_STEP, lo, hi)
	elif keycode == KEY_BRACKETLEFT:
		radius = clampf(radius / SCALE_STEP, lo, hi)
	ctl.remembered[ToolController.Tool.ROUNDABOUT] = {"radius": radius}
