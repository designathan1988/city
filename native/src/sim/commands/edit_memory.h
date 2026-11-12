// Allocation accounting for typed journal data; the caller shares one ledger across open/undo/redo roots.
#pragma once

#include "core/retained_memory.h"

namespace city {

struct EditEntry;
void account_entry(RetainedMemory &memory, const EditEntry &entry);

} // namespace city
