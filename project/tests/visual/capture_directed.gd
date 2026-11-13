## The directed runs of the visual capture: `--only=<name>` hands the whole run to one scene's shots.
class_name CaptureDirected
extends RefCounted


## True when `only` named one of them and it was shot; the run is over then.
static func run(cap: Node, only: PackedStringArray) -> bool:
	if only.has("transit"):
		await CaptureTransit.shots(cap)
	elif only.has("people"):
		await CapturePeople.shots(cap)
	elif only.has("shot"):
		await CapturePeople.bang(cap)
	elif only.has("lots"):
		await CaptureLots.shots(cap)
	elif only.has("clips"):
		await CapturePeople.clips(cap)
	elif only.has("sweep"):
		await CaptureSweep.shots(cap)
	else:
		return false
	return true
