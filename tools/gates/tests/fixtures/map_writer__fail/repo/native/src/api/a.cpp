// Exclusive holds of the map that name no writer.
#include <shared_mutex>
void typed(CityState &s) {
	std::unique_lock<std::shared_mutex> lk(s.map_mutex);
}
void deduced(CityState &s) {
	std::unique_lock held(s.map_mutex);
}
void by_hand(CityState &s) {
	s.map_mutex.lock();
}
void aliased(std::shared_mutex &m) {
	std::lock_guard<std::shared_mutex> lk(m);
}
