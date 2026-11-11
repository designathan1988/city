// A named writer holds the map exclusively; readers hold it shared; other mutexes are free.
void command(CityState &s) {
	map_hold::WriterHold lk(s.map_mutex, map_hold::Writer::COMMAND);
}
void read(const CityState &s) {
	std::shared_lock<std::shared_mutex> lk(s.map_mutex);
	std::lock_guard<std::mutex> tr(s.traffic_mutex);
	// std::unique_lock<std::shared_mutex> lk(s.map_mutex) in a comment holds nothing
}
