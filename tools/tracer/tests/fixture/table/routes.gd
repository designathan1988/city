# A table of the core's functions by command: a caller's command selects one entry.
const ROUTES := {&"seal": "seal_box", &"wipe": "wipe_box"}

var core = Core.new()

func send(kind):
	return core.callv(ROUTES[kind], [])

func seal():
	return send(&"seal")
