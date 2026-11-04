// The street's block of the save (save_street.cpp): the one call each of save_write.cpp and save_read.cpp makes.
#pragma once

namespace city {

struct ByteReader;
struct ByteWriter;
struct CityState;

void write_street(ByteWriter &w, const CityState &s);
bool read_street(ByteReader &r, CityState &s); // false on a corrupt block; a save without one leaves the street empty

} // namespace city
