// Fixture: a utility class in core whose own method changes its own field (encapsulation, not a foreign write).
#pragma once

namespace city {

struct Counter {
	int n = 0;
	void bump() { n++; }
};

} // namespace city
