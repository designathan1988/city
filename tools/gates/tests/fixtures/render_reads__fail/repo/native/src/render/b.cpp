// B.
void sync(CityState &s);
void rebuild(city::CityState *s);
void cast(const CityState &s) { auto &w = const_cast<CityState &>(s); }
