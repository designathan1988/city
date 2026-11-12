// A walker asks its setter; a local named like the member is no write of it.
void step(Walker &w) {
	enter(w, WalkMode::WAIT);
	const WalkMode mode = WalkMode::WAIT;
	if (w.mode == mode) return;
}

// A copy and a const reference read the walker's mode; neither writes it.
void look(const Walker &w) {
	const WalkMode &seen = w.mode;
	WalkMode copy = w.mode;
	if (seen == copy) return;
}

// Another body's member of the same name, written from a value: not the walker's.
void paint(Brush &b, int m) {
	b.mode = m;
}
