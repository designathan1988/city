// A building body as bytes, the same for equal bodies: the save's section of edited bodies, and the digest that tells two
// bodies apart. The maps are written by key order, so a copy of a body gives the same bytes.
#pragma once

#include "core/bytes.h"
#include "sim/buildings/b40/types.h"
#include "sim/buildings/building_store.h"

#include <cstdint>

namespace city {

void write_body(ByteWriter &w, const b40::Body &b);
bool read_body(ByteReader &r, b40::Body &b);
uint32_t body_digest(const b40::Body &b);

// The edited bodies of a city: the records that name one, each with its body. Only the versions in use are written.
void write_edited_bodies(ByteWriter &w, const BuildingStore &bs);
bool read_edited_bodies(ByteReader &r, BuildingStore &bs);

} // namespace city
