// The canonical text of a value, the one the tests compare with Python's (tests/test_engine_reader.py): `N`, `T`, `F`,
// `i<decimal>`, `s<UTF-8 hex>`, a tuple `(...)`, a list `[...]`, a dict `{k:v,...}`, a set `<...>` and a frozenset
// `f<...>` with their items' texts sorted, an object `@<class hex>{field:value,...}`.
#pragma once

#include <string>

#include "terms.h"

namespace ftengine {

std::string dump(const Store& store, Value value);

}  // namespace ftengine
