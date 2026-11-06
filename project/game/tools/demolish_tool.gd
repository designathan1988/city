## Bulldoze: a click removes the building under it, else the rubble, else the nearest road; a drag removes everything
## inside the rectangle between where the button went down and where it came up. The core refunds the roads' share and
## the result tells how much.
extends ToolBase

## A drag shorter than this is a click.
const DRAG_MIN_M := 2.0
const LINE_PX := 2.0

var _corner := Vector3.INF


func enter() -> void:
	ctl.hint.emit("", true)


func exit() -> void:
	ctl.preview.clear()


## The rectangle that would go, on the ground.
func drag(p: Vector3) -> void:
	if _corner == Vector3.INF or _corner.distance_to(p) <= DRAG_MIN_M:
		ctl.preview.clear()
		return
	var ring := PackedVector3Array()
	for at: Vector2 in [Vector2(_corner.x, _corner.z), Vector2(p.x, _corner.z), Vector2(p.x, p.z), Vector2(_corner.x, p.z), Vector2(_corner.x, _corner.z)]:
		ring.push_back(Vector3(at.x, CityStore.query(&"height_at", [at.x, at.y]), at.y))
	ctl.preview.show_ribbons([{"points": ring, "half_width": ctl.px_at(p) * LINE_PX}])


func click(p: Vector3) -> void:
	_corner = p


func in_progress() -> bool:
	return _corner != Vector3.INF


## The box being dragged goes, nothing demolished (Roadcraft bulldozer cancel).
func cancel() -> void:
	_corner = Vector3.INF
	ctl.preview.clear()


func release(p: Vector3) -> void:
	var from := _corner
	_corner = Vector3.INF
	ctl.preview.clear()
	if from == Vector3.INF:
		return
	var box := p != Vector3.INF and from.distance_to(p) > DRAG_MIN_M
	var r: Dictionary = CityStore.dispatch(&"demolish_box", [from, p]) if box else CityStore.dispatch(&"demolish", [from])
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)
		return
	var text := tr("game.toast.demolished.one") if not box else tr("game.toast.demolished.box")
	if r.money < 0:
		text += tr("game.toast.refunded").format({"money": Formatters.format_money(-r.money)})
	ctl.hint.emit(text, true)
