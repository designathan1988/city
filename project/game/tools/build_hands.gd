## What the Build tool holds besides its pick: a rectangle to drag, a core or a piece for the cut floor, a run to trace, the brush. One at a time: a card for one of them puts
## the others down.
class_name BuildHands
extends RefCounted

const FURNISH_CARDS := ["core", "piece", "turn"]
const PAINT_CARDS := ["brush", "copy_style", "finish"]

var mass: MassDrag
var furnish: FurnishHand
var run: RunHand
var brush: PaintBrush

var _ctl: ToolController


func _init(ctl: ToolController) -> void:
	_ctl = ctl
	mass = MassDrag.new(ctl)
	furnish = FurnishHand.new(ctl)
	run = RunHand.new(ctl)
	brush = PaintBrush.new(ctl)


func armed() -> bool:
	return mass.armed() or furnish.armed() or run.armed()


func disarm() -> void:
	mass.disarm()
	furnish.disarm()
	run.disarm()


## A card of the strip; true when one of the hands took it.
func act(spec: Dictionary, value: int, picked: Dictionary, report: Callable) -> bool:
	if spec.do != "mass":
		mass.disarm()
	if not FURNISH_CARDS.has(spec.do):
		furnish.disarm()
	if spec.do != "run":
		run.disarm()
	if not PAINT_CARDS.has(spec.do):
		brush.painting = false
	match spec.do:
		"core", "piece", "turn":
			report.call(furnish.act(spec, picked))
		"run":
			run.arm(spec)
			_tell(run.armed(), spec)
		"mass":
			mass.arm(value)
			_tell(mass.armed(), spec)
		"brush", "copy_style":
			brush.act(spec, value, picked)
		"finish":
			return brush.act(spec, value, picked)
		_:
			return false
	return true


func _tell(armed_now: bool, spec: Dictionary) -> void:
	if armed_now:
		_ctl.hint.emit(_ctl.tr("hint.builder.%s" % spec.id), true)


## A press with something in hand; true when it was for the hand.
func click(p: Vector3, picked: Dictionary, report: Callable) -> bool:
	if picked.is_empty():
		return false
	if mass.armed():
		mass.press(p, picked)
	elif furnish.armed():
		report.call(furnish.click(picked.id))
	elif run.armed():
		run.click(p)
	else:
		return false
	return true


func key(code: Key, picked: Dictionary, report: Callable) -> void:
	if picked.is_empty():
		return
	if furnish.armed() and code == KEY_R:
		report.call(furnish.turn(picked.id))
	elif run.armed() and (code == KEY_ENTER or code == KEY_KP_ENTER):
		report.call(run.finish(picked))
	elif run.armed() and code == KEY_BACKSPACE:
		run.back()
