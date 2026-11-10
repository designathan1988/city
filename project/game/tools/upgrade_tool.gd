## Upgrade (tab Melhorar): the profile picked in the catalogue goes on the road
## under the click; with Shift on its whole street (through the junctions, straightest continuation). The core charges
## the difference in value or refunds a share; the ribbon previews the new width over the segment.
extends ToolBase

var profile := 0
var _half := 0.0


func enter() -> void:
	profile = params.get("profile", 0)
	for p in CityStore.cached(&"road_profiles"):
		if p.index == profile:
			_half = p.width * 0.5
	ctl.hint.emit("", true)


func exit() -> void:
	ctl.preview.clear()


func hover(p: Vector3) -> void:
	var seg := road_under(p)
	if not seg.is_empty():
		ctl.preview.show_ribbon(seg.points, _half, not seg.ring)


func click(p: Vector3) -> void:
	if Input.is_key_pressed(KEY_ALT):
		var seg: Dictionary = CityStore.query(&"segment_at", [p])
		if seg.get("seg", -1) >= 0 and not seg.ring:
			ctl.set_tool(ToolController.Tool.UPGRADE, {"profile": seg.profile})
		return
	var r: Dictionary = CityStore.dispatch(&"upgrade_road", [p, profile, Input.is_key_pressed(KEY_SHIFT)])
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)
		return
	var text := Formatters.count_label("ui.insp.upgraded", r.segments) # the money joins it with " · ": no full stop
	if r.money != 0:
		text += " · " + Formatters.format_cost(r.money)
	ctl.hint.emit(text, true)
