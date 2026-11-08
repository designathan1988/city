## Zones › Zone: click or drag over the lots along the streets to zone them whole (Shift, or the eraser card, takes the zone off).
## The lot under the pointer is lit in the zone's colour, and the lots the stroke has passed stay lit until it is let go. Where the
## player holds no lot, the land lot there becomes theirs: a lot of the generated city, or the lot the streets propose (outlined by
## the zone layer while the tool is in hand). The core zones whole lots (paint_zone_path); the stroke is one undo step.
extends ToolBase

## A fast drag reports the pointer far apart: the whole way is sampled at this step, shorter than any lot's front.
const STEP_M := 2.0
## At most this many samples of one pointer move are sent, as one command.
const MOST_DABS := 400
## What the brush paints with Shift held: no zone.
const UNZONED := 0
## Two rings met by two samples are one lot when their middles round to the same decimetre (whichever corner each starts at: a
## proposed lot taken by the player starts at its front).
const MIDDLE_KEY_PER_M := 10.0

var zone := 1
var _last := Vector3.INF
var _painting := 0 # the zone of the stroke in hand
var _lit := {} # the lots the stroke passed, by their middle: their ground (lot_under cover)
var _zoned := 0 # lots the stroke zoned
var _met := false # the stroke met a lot


func enter() -> void:
	zone = params.get("zone", 1)
	ctl.hint.emit("", true)


func click(p: Vector3) -> void:
	_last = p
	_painting = UNZONED if Input.is_key_pressed(KEY_SHIFT) else zone
	_lit = {}
	_zoned = 0
	_met = false
	CityStore.dispatch(&"begin_stroke", ["cmd.place.zone"])
	_paint(PackedVector3Array([p]))


## The whole stroke is one undo step; what it did is said once, when it is let go, and only the lot under the pointer stays lit.
func release(p: Vector3) -> void:
	if _last == Vector3.INF:
		return
	CityStore.dispatch(&"end_stroke")
	_last = Vector3.INF
	_lit = {}
	if p == Vector3.INF:
		ctl.preview.clear()
	else:
		_show(p)
	if _zoned > 0:
		ctl.hint.emit(tr("hint.zone.removed" if _painting == UNZONED else "hint.zone.painted"), true)
	elif not _met:
		ctl.hint.emit(tr("hint.zone.empty"), false)


func drag(p: Vector3) -> void:
	if _last == Vector3.INF:
		return
	var steps := mini(ceili(p.distance_to(_last) / STEP_M), MOST_DABS)
	if steps < 1:
		return
	var dabs := PackedVector3Array()
	for k in range(1, steps + 1):
		dabs.append(_last.lerp(p, float(k) / steps))
	_paint(dabs)
	_last = p


func hover(p: Vector3) -> void:
	_show(p)


func exit() -> void:
	if _last != Vector3.INF:
		CityStore.dispatch(&"end_stroke")
	_last = Vector3.INF
	ctl.preview.clear()


## Escape lets the stroke go as it stands: what it zoned so far is one undo step.
func cancel() -> void:
	release(Vector3.INF)
	ctl.preview.clear()


func in_progress() -> bool:
	return _last != Vector3.INF


## The dabs of one pointer move as one command; the lots they met are lit until the stroke is let go.
func _paint(dabs: PackedVector3Array) -> void:
	for d in dabs:
		var lot: Dictionary = CityStore.query(&"lot_under", [d, _painting])
		if not lot.is_empty():
			_lit[_lot_key(lot.points)] = lot.cover
	var r: Dictionary = CityStore.dispatch(&"paint_zone_path", [dabs, _painting])
	_zoned += int(r.get("zoned_sides", 0))
	_met = _met or r.get("ok", false) or r.get("error", "") == "tools.toast.zoneSame"
	_show(dabs[-1])


## The stroke's lots and the one under the pointer, lit: the zone's colour, or the eraser's.
func _show(p: Vector3) -> void:
	var held := _last != Vector3.INF
	var painting := _painting if held else (UNZONED if Input.is_key_pressed(KEY_SHIFT) else zone)
	var swatch := ZoneStrip.swatch_of(painting)
	var fills := []
	for cover: PackedVector3Array in _lit.values():
		fills.append({"cover": cover, "colour": ThemeBuilder.tinted(swatch, "preview_zone_stroke_alpha")})
	var under: Dictionary = CityStore.query(&"lot_under", [p, painting])
	if not under.is_empty() and not _lit.has(_lot_key(under.points)):
		fills.append({"cover": under.cover, "colour": ThemeBuilder.tinted(swatch, "preview_zone_hover_alpha")})
	ctl.preview.show_fills(fills)


static func _lot_key(ring: PackedVector3Array) -> Vector2i:
	var sum := Vector3.ZERO
	for q in ring:
		sum += q
	sum /= ring.size()
	return Vector2i(roundi(sum.x * MIDDLE_KEY_PER_M), roundi(sum.z * MIDDLE_KEY_PER_M))
