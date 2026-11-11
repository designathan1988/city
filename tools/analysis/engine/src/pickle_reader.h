// The engine's input reader (log/trace-research.md F18): a pickle of protocol 5 (PEP 574, framed as PEP 3154 says) read
// into the store, the opcodes the tracer's facts use (each one as Lib/pickletools.py documents it); every container
// becomes a hash-consed term when the pickle hands it to its parent, which the pickle does only once it is whole.
#pragma once

#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>

#include "terms.h"

namespace ftengine {

struct PickleError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

// The value the pickle holds; throws PickleError on an opcode outside the subset or a malformed stream.
Value read_pickle(std::span<const uint8_t> data, Store& store);

}  // namespace ftengine
