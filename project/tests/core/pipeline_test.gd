## What streams into view brings its pipelines with its meshes (native/src/render/surface_data.h: a mesh made whole with its materials),
## so the engine makes none on the frame a thing is first drawn. The engine's own counts (RenderingServer.get_rendering_info: they only
## grow) of the compilations a frame about to be drawn triggers (surface) and of those made while drawing (draw) stand still while the
## camera goes from the whole town down to a street, to another district and out again; the count of those made with the meshes, off
## the frame, is the one that moves. Forward+: the counters are of the renderers that precompile (Godot docs, pipeline compilations).
## Run: godot --path project --rendering-method forward_plus res://tests/core/pipeline_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const SETTLE_LIMIT_S := 25.0 # real time the mesh jobs may take: StateWait
const SETTLE_FRAMES := 30
const FIRST_VIEW := [0.0, 0.0, 30.0, 35.0, 2600.0] # x, z, yaw, pitch, distance: the whole town, its far and mid levels
## The tour: a street from near (the near level, its parts, the street's furniture, trees and people), the blocks from mid distance,
## another district from near, and the town from far again.
const TOUR := [[80.0, -40.0, 210.0, 15.0, 60.0], [0.0, 0.0, 120.0, 25.0, 500.0], [300.0, 250.0, 120.0, 30.0, 90.0], [0.0, 0.0, 200.0, 25.0, 3000.0]]
const WARM_TICKS := 600 # the fleet and the walkers on the streets before the tour


func _ready() -> void:
	if not boot():
		finish()
		return
	var rig := CityCamera.new()
	add_child(rig)
	new_map(TOWN_MAP, SEED, true)
	Settings.attach(world, get_viewport())
	ticks(WARM_TICKS)
	await _stand(rig, FIRST_VIEW)
	var first := _counts()
	print("first view: ", first)
	for view: Array in TOUR:
		await _stand(rig, view)
		print("view ", view, ": ", _counts())
	var last := _counts()
	check(last.mesh > first.mesh, "the tour's meshes brought pipelines of their own, made off the frame: %d" % (last.mesh - first.mesh))
	check(last.surface == first.surface, "no pipeline made on the frame a thing first appeared: %d" % (last.surface - first.surface))
	check(last.draw == first.draw, "no pipeline made while drawing: %d" % (last.draw - first.draw))
	finish()


func _stand(rig: CityCamera, view: Array) -> void:
	rig.jump(Vector3(view[0], 0.0, view[1]), view[2], view[3], view[4])
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	await StateWait.drawn(get_tree(), SETTLE_LIMIT_S)
	for i in SETTLE_FRAMES:
		await get_tree().process_frame


func _counts() -> Dictionary:
	return {
		"mesh": RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_MESH),
		"surface": RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_SURFACE),
		"draw": RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_DRAW),
		"specialization": RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_PIPELINE_COMPILATIONS_SPECIALIZATION),
	}
