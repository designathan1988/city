# A script reading members of objects it knows nothing of, and of one declared with a type it does not define.
func press(core, thing):
	core.fire(1)
	thing.lift(2)

func nudge(body: Body):
	body.fire(3)

var table = {"fire": lift}

const ROUTES = {&"shoot": "fire", &"tidy": "area/sweep"}
var routes = {}

func setup(core):
	for name in ROUTES:
		routes[name] = Callable(core, ROUTES[name])

func send(command):
	return routes[command].callv([])

func has(key):
	return routes.has("fire") or FileAccess.file_exists("res://fire/sweep.json")

var names = []

func note():
	names.append("fire")

func count():
	return names.size()

func ask(store):
	var r = store.send("fire")
	return r.is_empty()

func ask_any(store, command):
	return store.send(command)

func later():
	call_deferred("lift")

func say():
	print("no function is named by a sentence: fire")

func lift(n):
	return n
