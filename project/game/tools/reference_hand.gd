## Construir › Massa › Referência 3D: the cards on the model, its file dialog, and the building read off it (the 4.0's loadReference and buildReference).
class_name ReferenceHand
extends RefCounted

var model: ReferenceModel

var _ctl: ToolController
var _picked := {}
var _dialog: FileDialog
var _build_guard: InputSequence.Guard


func _init(ctl: ToolController) -> void:
	_ctl = ctl
	model = ReferenceModel.new()
	ctl.add_child(model)


## The tool owns both the imported model and its dialog, including a dialog that was never opened again.
func close() -> void:
	_cancel_build()
	if is_instance_valid(_dialog):
		_dialog.hide()
		_dialog.file_selected.disconnect(open)
		_dialog.queue_free()
	_dialog = null
	if is_instance_valid(model):
		model.queue_free()
	model = null
	_picked.clear()


func act(spec: Dictionary, picked: Dictionary) -> Dictionary:
	_picked = picked
	if spec.do == "ref_load":
		_ask_file()
		return {"ok": true}
	if not model.loaded:
		return {"ok": false, "error": "hint.reference.none"}
	if spec.do in ["ref_turn", "ref_flip", "ref_remove"]:
		_cancel_build()
	match spec.do:
		"ref_build":
			build()
		"ref_align":
			align()
		"ref_turn":
			model.turn()
		"ref_flip":
			model.flip()
		"ref_fade":
			model.fade(spec.value)
		"ref_toggle":
			model.toggle()
		"ref_remove":
			model.clear()
	return {"ok": true}


func open(path: String) -> bool:
	_cancel_build()
	if not model.load_file(path):
		_ctl.hint.emit(_ctl.tr("hint.reference.failed"), false)
		return false
	align()
	_ctl.hint.emit(_ctl.tr("hint.reference.loaded").format({"name": model.file_name}), true)
	return true


## Over the building picked, on the middle of its plan.
func align() -> void:
	_cancel_build()
	if _picked.is_empty():
		return
	var d: Dictionary = CityStore.query(&"body_draft", [_picked.id, {}])
	model.place_on(d.cx, d.cz, d.y, d.rot)


## The building the model shows, where the model stands, in place of the building picked.
func _cancel_build() -> void:
	if _build_guard != null:
		_build_guard.live = false
	_build_guard = null


func build() -> Dictionary:
	_cancel_build()
	var guard := InputSequence.Guard.new()
	_build_guard = guard
	var source := model
	var generation: int = CityStore.snapshot.get("city_generation", -1)
	var replaced: int = _picked.get("id", -1)
	var target: Dictionary = CityStore.query(&"reference_target", [replaced])
	var read: Dictionary = await source.capture()
	if not guard.live or not is_instance_valid(source) or source != model or not is_instance_valid(_ctl) or generation != CityStore.snapshot.get("city_generation", -1):
		return {"ok": false, "error": "hint.reference.failed"}
	var r: Dictionary
	if read.has("error"):
		r = {"ok": false, "error": read.error}
	else:
		var at: Dictionary = read.frame
		r = await CityStore.ordered_dispatch(&"build_reference", [read.triangles, read.views, at.x, at.z, at.rot, replaced, target], guard)
	if not guard.live or not is_instance_valid(source) or source != model or not is_instance_valid(_ctl) or generation != CityStore.snapshot.get("city_generation", -1):
		return {"ok": false, "cancelled": true}
	if r.get("cancelled", false):
		if _build_guard == guard:
			_cancel_build()
		return r
	if _build_guard == guard:
		_build_guard = null
	if r.get("ok", false):
		_ctl.hint.emit(_ctl.tr("hint.reference.built"), true)
	else:
		_ctl.hint.emit(_ctl.tr(r.get("error", "hint.reference.empty")), false)
	return r


func _ask_file() -> void:
	if _dialog == null:
		_dialog = FileDialog.new()
		_dialog.access = FileDialog.ACCESS_FILESYSTEM
		_dialog.file_mode = FileDialog.FILE_MODE_OPEN_FILE
		_dialog.use_native_dialog = true
		_dialog.filters = PackedStringArray(["*.glb,*.gltf"])
		_dialog.file_selected.connect(open)
		_ctl.add_child(_dialog)
	_dialog.popup_centered()
