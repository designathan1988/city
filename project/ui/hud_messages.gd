## The shell's two messages: the hint bar under the top bar (the price while aiming and refusals; what a tool does is its drawer's note,
## as in the reference) and the toast above it, the game's answers (saved, refused, undone) that go by themselves.
class_name HudMessages
extends Node

var _hint: Label
var _toast: PanelContainer
var _toast_label: Label
var _toast_left := 0.0


func _init(root: Control) -> void:
	var bar := _bar(root, HudWidgets.px("toast_top") + HudWidgets.px("hud_h"))
	_hint = HudWidgets.label(bar, &"BodyLabel")
	_hint.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	_toast = _bar(root, HudWidgets.px("toast_top"))
	_toast_label = HudWidgets.label(_toast, &"BodyLabel")
	for l: Label in [_hint, _toast_label]: # the text arrives translated (ToolController.hint): a key that slipped through shows as one
		l.auto_translate_mode = Node.AUTO_TRANSLATE_MODE_DISABLED


## A panel hanging from the top edge, centred, hidden until it has something to say.
static func _bar(root: Control, top: float) -> PanelContainer:
	var p := HudWidgets.panel(root, &"Toast")
	p.anchor_left = 0.5
	p.anchor_right = 0.5
	p.grow_horizontal = Control.GROW_DIRECTION_BOTH
	p.offset_top = top
	p.hide()
	return p


func hint(text: String, ok: bool) -> void:
	var p := _hint.get_parent() as PanelContainer
	p.theme_type_variation = &"Toast" if ok else &"DangerToast"
	_hint.theme_type_variation = &"BodyLabel" if ok else &"DangerLabel"
	_hint.text = text
	p.visible = text != ""
	p.reset_size()
	p.offset_left = -p.size.x * 0.5
	p.offset_right = p.size.x * 0.5


func flash(text: String, ok: bool) -> void:
	_toast.theme_type_variation = &"Toast" if ok else &"DangerToast"
	_toast_label.text = text
	_toast.show()
	_toast.reset_size()
	_toast.offset_left = -_toast.size.x * 0.5
	_toast.offset_right = _toast.size.x * 0.5
	_toast_left = float(DataTables.ui("hud").timings.toast_s)


func _process(delta: float) -> void:
	if _toast_left > 0.0:
		_toast_left -= delta
		_toast.visible = _toast_left > 0.0
