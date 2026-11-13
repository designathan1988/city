## The busy lit crossing of the visual capture (traffic_probe busy_crossing): where the camera stands is defined here, in the
## crossing's own frame, so a photo of one thing is always taken from the same place and at the same target.
class_name CaptureCrossing
extends RefCounted

## Camera offsets from the centre of the crossing: an oblique view and a closer one (the target is the centre).
const FROM := Vector3(-8, 42, 16)
const NEAR_FROM := Vector3(-6, 22, 10)


## `cap` is the capture scene (its _shot); `cross` the probe's crossing.
static func views(cap: Node, cross: Vector3) -> void:
	await cap._shot("15_crosswalk", cross + FROM, cross)
	await cap._shot("16_crosswalk_near", cross + NEAR_FROM, cross)


## The close view again under each regional paint standard of Menu > Paint, by day, the paint the only thing that changes:
## the dashes of the arms, the lines and the zebra. The map goes back to its first standard.
static func paint(cap: Node, cross: Vector3) -> void:
	await CaptureSignals.at_hour(cap, CaptureViews.DAY)
	var styles: Array = DataTables.render().streets.markings.styles
	for i in styles.size():
		CityStore.dispatch(&"set_marking_style", [i, false])
		await cap._shot("16_paint_%s" % styles[i].id, cross + NEAR_FROM, cross)
	CityStore.dispatch(&"set_marking_style", [0, false])
