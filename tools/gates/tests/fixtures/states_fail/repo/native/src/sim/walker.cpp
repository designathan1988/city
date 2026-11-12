// A walker's mode written outside its setter.
void step(Walker &w) {
	w.mode = WalkMode::WAIT;
}

// ... and through a walker held by reference or by pointer, from a value the statement does not name.
void hold(Walker &w, WalkMode m) {
	w.mode = m;
}

void park(Walker *w, WalkMode m) {
	w->mode = m;
}

// ... and through a reference bound to it, then written by its own name.
void lend(Walker &w) {
	WalkMode &m = w.mode;
	m = WalkMode::WAIT;
}
