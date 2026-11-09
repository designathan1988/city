// Fixture: the target's owner of cell keys.
#pragma once

#include <cstdint>

namespace city {

inline int64_t cell_key(int cx, int cy) { return (int64_t(cx) << 32) ^ int64_t(uint32_t(cy)); }

} // namespace city
