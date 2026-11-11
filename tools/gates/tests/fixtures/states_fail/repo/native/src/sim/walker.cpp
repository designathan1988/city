// A walker's mode written outside its setter.
void step(Walker &w) {
	w.mode = WalkMode::WAIT;
}
