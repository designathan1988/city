## A wait on what runs on threads of its own (a save, the mesh jobs, a hand-over's clock): those run in real time whatever the
## frames' pace, and the tests run in simulated time (tools/tasks/suites.json: --fixed-fps, uncapped), so frames say nothing of how
## long they had. The wait is on the state; its guard against a hang is real time, the time the old frame counts gave at 60 Hz.
class_name StateWait
extends RefCounted

## Frames on `tree` until `done` returns true, at most `limit_s` seconds of real time. True when it came.
static func until(tree: SceneTree, done: Callable, limit_s: float) -> bool:
	var started := Time.get_ticks_msec()
	while not done.call():
		if float(Time.get_ticks_msec() - started) / 1000.0 >= limit_s:
			return false
		await tree.process_frame
	return true


## The renderer has nothing left to draw of the view as the caller left it (render_pending), at most `limit_s` seconds of real time.
## The renderer learns the camera only in the frame's processing (CityCore._process), and process_frame comes before it: the first
## await reaches the frame that sees the camera as left, the second resumes after that frame's sync; only then is the answer this view's.
static func drawn(tree: SceneTree, limit_s: float) -> bool:
	await tree.process_frame
	await tree.process_frame
	return await until(tree, func() -> bool: return int(CityStore.query(&"render_pending")) == 0, limit_s)
