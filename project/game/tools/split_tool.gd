## Split (tab Dividir): a new node on the road where it is clicked (not within
## edit_split_min_m of its ends). The marker shows where the cut falls.
extends ToolBase

const MARK_SHARE := 0.6


func enter() -> void:
	ctl.hint.emit("", true)


func exit() -> void:
	ctl.preview.clear()


func hover(p: Vector3) -> void:
	var seg := road_under(p)
	if not seg.is_empty():
		ctl.preview.show_disc(seg.at, seg.half_width * MARK_SHARE, true)


func click(p: Vector3) -> void:
	var r: Dictionary = CityStore.dispatch(&"split_road", [p])
	ctl.hint.emit(tr("ui.insp.split") if r.get("ok", false) else refusal(r), r.get("ok", false))
