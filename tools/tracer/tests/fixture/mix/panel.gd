# A script reading members of objects it knows nothing of, and of one declared with a type it does not define.
func press(core, thing):
	core.fire(1)
	thing.lift(2)

func nudge(body: Body):
	body.fire(3)

var table = {"fire": lift}

func lift(n):
	return n
