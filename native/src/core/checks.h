// The checks the tests' DLL makes and the export's DLL leaves out (CITY_CHECKS, defined for template_debug in native/CMakeLists.txt):
// a rule of the core whose test would cost a branch in a hot path is held there. city_stop says what was broken and ends the process
// where it happened, so the failure is read on the stack of the first run that breaks the rule, not when its effects show.
#pragma once

#include <cstdio>
#include <cstdlib>

namespace city {

#ifdef CITY_CHECKS
constexpr bool CHECKS = true;
#else
constexpr bool CHECKS = false;
#endif

[[noreturn]] inline void city_stop(const char *what) {
	std::fprintf(stderr, "citycore stopped: %s\n", what);
	std::fflush(stderr);
	std::abort();
}

} // namespace city
