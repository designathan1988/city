## The wait every capture makes before it photographs: until the world has drawn everything it was asked to. A photo that
## leaves with drawing pending is a failure of the capture: counted here, and the capture's exit code says it (exit_code).
class_name CaptureWait
extends RefCounted

## Most frames waited for the drawing to finish; a photo that leaves at this limit is incomplete.
const BUILD_FRAMES := 400

## Photos of this run that came out incomplete.
static var incomplete := 0


## Waits `settle` frames (the world notices the camera and queues what it wants), then until nothing is left to draw.
## Returns the frames of the second wait. Every wait is logged, and a photo that leaves with work pending is counted: a
## building not yet drawn is a photo that lies.
static func until_drawn(host: Node, settle: int, label: String) -> int:
	var tree := host.get_tree()
	for i in settle:
		await tree.process_frame
	var waited := 0
	while CityStore.query(&"render_pending") > 0 and waited < BUILD_FRAMES:
		await tree.process_frame
		waited += 1
	var pending: int = CityStore.query(&"render_pending")
	var cost: Dictionary = CityStore.query(&"render_cost")
	print("espera: %s quadros=%d pendente=%d pedir=%d sem_vez=%d pedidos=%d prontos=%d orcamento=%d fila=%s substituidos=%d" % [label, waited, pending,
			cost.b_want, cost.b_unmet, cost.b_requested, cost.b_done, cost.budget_left, cost.backlog, cost.jobs_replaced])
	if pending > 0:
		mark_incomplete("%s came out with %d drawing jobs pending after %d frames" % [label, pending, waited])
	return waited


## A photo that came out without all it should show: told and counted.
static func mark_incomplete(why: String) -> void:
	incomplete += 1
	push_warning("INCOMPLETE PHOTO: " + why)


## The exit code a capture quits with: 0 only when it counted no failure of its own and every photo was complete.
static func exit_code(failures: int) -> int:
	if incomplete > 0:
		print("FAIL %d incomplete photos" % incomplete)
	return 1 if failures + incomplete > 0 else 0
