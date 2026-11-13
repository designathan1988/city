## The waits of a look plan (look.gd's "until" and "blastShot"): frames until the game's own answer is the one asked, at most a
## deadline of real time. Each says what it waits for and its limit when it starts, the answer it has every PROGRESS_MS of real
## time while it waits, and past its deadline prints a FAIL line, so a wait that never ends names itself before the task's deadline.
class_name LookWait
extends RefCounted

## Real time between two progress lines of a wait, ms.
const PROGRESS_MS := 3000


## Frames on `tree` until `reached` holds of `ask`'s answer, at most `limit_ms` of real time; `what` names the wait in the log.
## True when it came; false, with a FAIL line, past the limit (the caller counts the failure).
static func until(tree: SceneTree, what: String, ask: Callable, reached: Callable, limit_ms: float) -> bool:
	print("wait: %s (at most %d ms)" % [what, limit_ms])
	var started := Time.get_ticks_msec()
	var told := started
	var answer: Variant = ask.call()
	while not reached.call(answer):
		var now := Time.get_ticks_msec()
		if now - started > limit_ms:
			print("FAIL wait: %s past %d ms (the answer: %s)" % [what, limit_ms, answer])
			return false
		if now - told >= PROGRESS_MS:
			told = now
			print("wait: %s, %d ms in (the answer: %s)" % [what, now - started, answer])
		await tree.process_frame
		answer = ask.call()
	print("wait: %s, reached in %d ms" % [what, Time.get_ticks_msec() - started])
	return true
