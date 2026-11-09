// The ECMAScript built-ins a translated program calls, with ECMAScript's semantics: what Number.prototype.toFixed,
// String.prototype.slice or a Map's iteration order give in JavaScript, these give in C++. The translator writes every
// built-in the same way, as a call here; each is checked against Node (test/js-runtime.test.ts). Header only, C++17.
#pragma once

#include "js_array.h"
#include "js_map.h"
#include "js_number.h"
#include "js_object.h"
#include "js_random.h"
#include "js_string.h"
#include "js_value.h"

#include <cstdio>
#include <cstdlib>
#include <string>

namespace js {

/** A type that takes no part in deducing a template's parameters: a callback, whose type comes from the other arguments. */
template <class T> struct same_type { using type = T; };
template <class T> using same = typename same_type<T>::type;

/**
 * A frozen object (ECMA-262 20.1.2.6 Object.freeze, SetIntegrityLevel "frozen"): its state is fixed when it is made and it
 * holds it itself; it gives its value only as const (to read, to walk, to copy out), so no write to it compiles wherever it
 * is held: the guarantee freezing gives, which TypeScript states by typing Object.freeze's result Readonly<T>.
 */
template <class T>
class Frozen {
public:
	explicit Frozen(T value) : value_(std::move(value)) {}
	operator const T &() const { return value_; }
	const T &get() const { return value_; }
	const T *operator->() const { return &value_; }
	auto begin() const { return value_.begin(); }
	auto end() const { return value_.end(); }
private:
	T value_;
};

/** Object.freeze(o): o, frozen. */
template <class T> Frozen<T> object_freeze(T o) { return Frozen<T>(std::move(o)); }

/** A `throw` of the source: a promise of the program broken, which the target never recovers from. */
[[noreturn]] inline void fail(const std::string &why) {
	std::fputs(why.c_str(), stderr);
	std::fputc('\n', stderr);
	std::abort();
}

} // namespace js
