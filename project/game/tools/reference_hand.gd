## Build › Mass › 3D reference: the cards on the model, its file dialog, and the building read off it (the 4.0's loadReference and buildReference).
class_name ReferenceHand
extends RefCounted

var model: ReferenceModel

var _ctl: ToolController
var _picked := {}
var _dialog: FileDialog


func _init(ctl: ToolController) -> void:
	_ctl = ctl
	model = ReferenceModel.new()
	ctl.add_child(model)


func act(spec: Dictionary, picked: Dictionary) -> Dictionary:
	_picked = picked
	if spec.do == "ref_load":
		_ask_file()
		return {"ok": true}
	if not model.loaded:
		return {"ok": false, "error": "hint.reference.none"}
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
	if not model.load_file(path):
		_ctl.hint.emit(_ctl.tr("hint.reference.failed"), false)
		return false
	align()
	_ctl.hint.emit(_ctl.tr("hint.reference.loaded").format({"name": model.file_name}), true)
	return true


## Over the building picked, on the middle of its plan.
func align() -> void:
	if _picked.is_empty():
		return
	var d: Dictionary = CityStore.query(&"body_draft", [_picked.id, {}])
	model.place_on(d.cx, d.cz, d.y, d.rot)


## The building the model shows, where the model stands, in place of the building picked.
func build() -> Dictionary:
	var read: Dictionary = await model.capture()
	var at: Dictionary = model.frame()
	var replaced: int = _picked.get("id", -1)
	var r: Dictionary = CityStore.dispatch(&"build_reference", [read.triangles, read.views, at.x, at.z, at.rot, replaced])
	if r.get("ok", false):
		_ctl.hint.emit(_ctl.tr("hint.reference.built"), true)
	else:
		_ctl.hint.emit(_ctl.tr(r.get("error", "hint.reference.empty")), false)
	return r


## The Build tool put away: the model and its file dialog go with it (both are the tool controller's children while the tool is up).
func retire() -> void:
	model.queue_free()
	if _dialog != null:
		_dialog.queue_free()
		_dialog = null


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
