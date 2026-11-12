// A walker asks its setter; a local named like the member is no write of it.
void step(Walker &w) {
	enter(w, WalkMode::WAIT);
	const WalkMode mode = WalkMode::WAIT;
	if (w.mode == mode) return;
}

// Another body's member of the same name, written from a value: not the walker's.
void paint(Brush &b, int m) {
	b.mode = m;
}
