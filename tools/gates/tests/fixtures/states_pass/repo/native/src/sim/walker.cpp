// A walker asks its setter; a local named like the member is no write of it.
void step(Walker &w) {
	enter(w, WalkMode::WAIT);
	const WalkMode mode = WalkMode::WAIT;
	if (w.mode == mode) return;
}
