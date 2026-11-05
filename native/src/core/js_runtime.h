// The ECMAScript built-ins a translated program calls, with ECMAScript's semantics: what Number.prototype.toFixed,
// String.prototype.slice or a Map's iteration order give in JavaScript, these give in C++. The translator writes every
// built-in the same way, as a call here; each is checked against Node (test/js-runtime.test.ts). Header only, C++17.
#pragma once

#include "core/js_array.h"
#include "core/js_map.h"
#include "core/js_number.h"
#include "core/js_string.h"

#include <cstdio>
#include <cstdlib>
#include <string>

namespace js {

/** A type that takes no part in deducing a template's parameters: a callback, whose type comes from the other arguments. */
template <class T> struct same_type { using type = T; };
template <class T> using same = typename same_type<T>::type;

/** A `throw` of the source: a promise of the program broken, which the target never recovers from. */
[[noreturn]] inline void fail(const std::string &why) {
	std::fputs(why.c_str(), stderr);
	std::fputc('\n', stderr);
	std::abort();
}

} // namespace js
