## Fixture store: sends a command, by name, to the native core.
extends Node

signal changed

var _core


func _ready() -> void:
	_core = ClassDB.instantiate(&"Core")
	_core.built.connect(_on_built)


func dispatch(command: StringName, args: Array = []) -> Variant:
	var result: Variant = Callable(_core, command).callv(args)
	changed.emit()
	return result


func _on_built() -> void:
	pass
