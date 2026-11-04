## Inspect: a click picks the building or road there and the HUD's inspector shows what
## the core reports about it.
extends ToolBase


func enter() -> void:
	ctl.hint.emit("", true)


func click(p: Vector3) -> void:
	var info: Dictionary = CityStore.query(&"inspect", [p])
	info["at"] = p
	ctl.inspected.emit(info)


func hover(p: Vector3) -> void:
	ctl.hovered.emit(CityStore.query(&"inspect", [p]))


func cancel() -> void:
	ctl.inspected.emit({})


func exit() -> void:
	ctl.inspected.emit({})
