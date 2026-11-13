## The wait every capture makes before it photographs: until the world has drawn everything it was asked to. A photo that
## leaves with drawing pending is a failure of the capture: counted here, and the capture's exit code says it (exit_code).
class_name CaptureWait
extends RefCounted

## Most real time waited for the drawing to finish (400 frames of the old 60 Hz loop): a photo that leaves at this limit is incomplete.
const BUILD_LIMIT_S := 6.7
## The frozen photo: frames between two looks at the picture, and how many looks it may take to hold still (a pipeline compiled
## in the background replaces the ubershader's drawing on a frame the machine decides: Godot docs, pipeline compilations).
const STEADY_FRAMES := 16
const STEADY_LOOKS := 30
## Most real time the frozen photo waits to hold still: the looks' frames (STEADY_FRAMES * STEADY_LOOKS) at the floor's 30 fps
## (doc/specification.md), so a machine slow to draw or to read the picture back ends the wait by the clock, not by the frames.
const STEADY_LIMIT_S := 16.0

## Photos of this run that came out incomplete.
static var incomplete := 0


## Waits `settle` frames (the world notices the camera and queues what it wants), then until nothing is left to draw.
## Returns the real time of the second wait, ms. Every wait is logged, and a photo that leaves with work pending is counted: a
## building not yet drawn is a photo that lies.
static func until_drawn(host: Node, settle: int, label: String) -> int:
	var tree := host.get_tree()
	for i in settle:
		await tree.process_frame
	var started := Time.get_ticks_msec()
	await StateWait.drawn(tree, BUILD_LIMIT_S)
	var waited := Time.get_ticks_msec() - started
	var pending: int = CityStore.query(&"render_pending")
	var cost: Dictionary = CityStore.query(&"render_cost")
	print("wait: %s ms=%d pending=%d want=%d unmet=%d asked=%d done=%d budget=%d backlog=%s replaced=%d" % [label, waited, pending,
			cost.b_want, cost.b_unmet, cost.b_requested, cost.b_done, cost.budget_left, cost.backlog, cost.jobs_replaced])
	if pending > 0:
		mark_incomplete("%s came out with %d drawing jobs pending after %d ms" % [label, pending, waited])
	return waited



## Frames until the picture holds still (two looks STEADY_FRAMES apart give the same bytes), so whatever settles after the
## drawing is done settles before the photo. False, told with the limit that ended it, past STEADY_LOOKS or STEADY_LIMIT_S.
static func until_steady(host: Node, label: String) -> bool:
	print("wait: %s to hold still (two looks %d frames apart alike; at most %d looks and %.0f s)" % [label, STEADY_FRAMES,
			STEADY_LOOKS, STEADY_LIMIT_S])
	var started := Time.get_ticks_msec()
	var last := PackedByteArray()
	for i in STEADY_LOOKS:
		if float(Time.get_ticks_msec() - started) / 1000.0 >= STEADY_LIMIT_S:
			print("FAIL the picture of %s never held still: past the limit of %.0f s, after %d looks" % [label, STEADY_LIMIT_S, i])
			return false
		for f in STEADY_FRAMES:
			await host.get_tree().process_frame
		await RenderingServer.frame_post_draw
		var now := host.get_viewport().get_texture().get_image().get_data()
		if now == last:
			print("steady: %s after %d looks, %d ms" % [label, i + 1, Time.get_ticks_msec() - started])
			return true
		last = now
	print("FAIL the picture of %s never held still: past the limit of %d looks, after %d ms" % [label, STEADY_LOOKS,
			Time.get_ticks_msec() - started])
	return false


## A photo that came out without all it should show: told and counted.
static func mark_incomplete(why: String) -> void:
	incomplete += 1
	push_warning("INCOMPLETE PHOTO: " + why)


## The exit code a capture quits with: 0 only when it counted no failure of its own and every photo was complete.
static func exit_code(failures: int) -> int:
	if incomplete > 0:
		print("FAIL %d incomplete photos" % incomplete)
	return 1 if failures + incomplete > 0 else 0
