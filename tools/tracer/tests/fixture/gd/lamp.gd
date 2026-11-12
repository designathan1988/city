# A choice between functions, a cast, and a sum that is no function.
func choose(up):
	var pick = polish_a if up else polish_b
	register(pick)

func tally():
	register(polish_a + 1)

func recast(x):
	return (x as Crate).lift(1)

func wire_a(a):
	a.done.connect(polish_a)

func wire_b(b):
	b.done.connect(polish_b)

func wire_c(c):
	c.done.connect(hide)

func fire(a):
	a.done.emit()

func look(event):
	register(event.done)

var kinds = {"run_it": polish_a}

func copy(src, dst):
	dst.other = src.run_it

func use(x):
	x.other()

func first(a):
	a.done[0].call()

func drift(event):
	var seen = event.done
	seen.is_empty()

func glance(thing):
	register(thing.lift)

func polish_a():
	return 1

func polish_b():
	return 2
