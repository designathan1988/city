// The one owner of the map's exclusive hold.
class WriterHold {
	std::unique_lock<std::shared_mutex> lock_;
};
