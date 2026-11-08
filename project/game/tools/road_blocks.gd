## Streets by the block in the road tool: the grid as the player left it, its preview and laying it.
class_name RoadBlocks
extends RefCounted


static func settings() -> Dictionary:
	if Settings.road.blocks.cols == 0:
		var d: Dictionary = DataTables.ui("hud").tool_options.road_blocks
		Settings.road.blocks.merge({"cols": int(d.cols), "rows": int(d.rows), "size": float(d.size_m), "turn": 0.0}, true)
	return Settings.road.blocks


static func lay(tool: RefCounted, at: Vector3) -> void:
	var b := settings()
	var r: Dictionary = CityStore.dispatch(&"build_blocks", [at, int(b.cols), int(b.rows), float(b.size), deg_to_rad(b.turn), tool.drawn])
	b.armed = false
	tool._clear_preview()
	tool.ctl.hint.emit(TranslationServer.translate("tools.toast.blocksBuilt" if r.get("ok", false) else "hint.road.invalid"), true)
	tool.ctl.options_changed.emit()


static func show(ctl: ToolController, at: Vector3) -> void:
	var b := settings()
	var roads := []
	for line: PackedVector3Array in CityStore.query(&"block_lines", [at, int(b.cols), int(b.rows), float(b.size), deg_to_rad(b.turn), DataTables.ui("hud").tool_options.road_blocks.preview_step_m]):
		roads.push_back({"points": line, "half_width": ctl.px_at(at) * DataTables.ui("hud").tool_options.road_blocks.preview_half_px})
	ctl.preview.show_ribbons(roads)
