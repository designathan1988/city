## The wait every capture makes before it photographs: until the world has drawn everything it was asked to.
class_name CaptureWait
extends RefCounted

## Most frames waited for the drawing to finish; a photo that leaves at this limit is announced as incomplete.
const BUILD_FRAMES := 400


## Waits `settle` frames (the world notices the camera and queues what it wants), then until nothing is left to draw.
## Returns the frames of the second wait. Every wait is logged, and a photo that leaves with work pending says so: a
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
	print("espera: %s quadros=%d pendente=%d" % [label, waited, pending])
	if pending > 0:
		push_warning("FOTO INCOMPLETA: %s saiu com %d trabalhos de desenho pendentes depois de %d quadros" % [label, pending, waited])
	return waited
