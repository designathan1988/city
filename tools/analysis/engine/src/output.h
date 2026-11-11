// The engine's result written for Python (featuretrace/engine.py reads it with pickle): the links, every object's key,
// the objects each active function returns and each name read in it holds; a pickle of protocol 4 (PEP 3154), its
// shared tuples memoized once.
#pragma once

#include <string>

#include "analysis.h"

namespace ftengine {

// The result's bytes. Wires the return node of every active run first (and, with `names`, the node of each of its
// locals and parameters) and runs to the fixpoint again, as the Python Result does when it is asked.
std::string result_pickle(Analysis& an, class Semantics& semantics, bool names);

}  // namespace ftengine
