// A.
void build(const CityState &s) {
	std::shared_lock<std::shared_mutex> map(s.map_mutex);
}
