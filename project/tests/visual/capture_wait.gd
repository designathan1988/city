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
## drawing is done settles before the photo. False, told, past STEADY_LOOKS.
static func until_steady(host: Node, label: String) -> bool:
	var last := PackedByteArray()
	for i in STEADY_LOOKS:
		for f in STEADY_FRAMES:
			await host.get_tree().process_frame
		await RenderingServer.frame_post_draw
		var now := host.get_viewport().get_texture().get_image().get_data()
		if now == last:
			print("steady: %s after %d looks" % [label, i + 1])
			return true
		last = now
	print("FAIL the picture of %s never held still (%d looks)" % [label, STEADY_LOOKS])
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
