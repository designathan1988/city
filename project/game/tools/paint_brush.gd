## Construir › Cores: the brush. Pintar turns clicking into painting with what the brush holds; a finish card loads it, and Copiar estilo loads what the pick is made of.
## With the brush off a finish card paints the pick at once.
class_name PaintBrush
extends RefCounted

var painting := false
var held := {}

var _ctl: ToolController


func _init(ctl: ToolController) -> void:
	_ctl = ctl


## A card of the Paint tab; false when the pick is still to be painted by it.
func act(spec: Dictionary, value: int, picked: Dictionary) -> bool:
	match spec.do:
		"brush":
			painting = not painting
			_ctl.hint.emit(_ctl.tr("hint.builder.paint"), true)
			return true
		"copy_style":
			if picked.is_empty():
				_ctl.hint.emit(_ctl.tr("hint.builder.select"), false)
				return true
			held = CityStore.query(&"body_paint_at", [picked.id, picked.volume, maxi(picked.side, 0), picked.storey])
			painting = true
			_ctl.hint.emit(_ctl.tr("builder.styleCopied"), true)
			return true
	held = {"finish": value, "rgb": spec.rgb}
	return painting


## What a click does with the brush on: the face hit takes what it holds.
func paint(hit: Dictionary, scope: int) -> Dictionary:
	return BodyEdits.run({"do": "finish", "rgb": held.rgb}, held.finish, hit, scope)


func on() -> bool:
	return painting and not held.is_empty()
