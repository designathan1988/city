// A clock's pace: an atomic member stored only by its setter, through its table.
enum class Pace : int { STILL, GO };
constexpr unsigned PACE_MOVES[2] = { 0b10, 0b01 };
struct Clock {
	std::atomic<Pace> pace_{ Pace::STILL };
	void enter(Pace next) {
		if (PACE_MOVES[int(pace_.load())] & (1u << int(next))) pace_.store(next);
	}
	bool still() const { return pace_.load() == Pace::STILL; }
};
