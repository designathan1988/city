// The walker's mode and its one setter.
enum class WalkMode : int { WALK, WAIT };
constexpr bool WALK_MOVES[2][2] = { { false, true }, { true, false } };
void enter(Walker &w, WalkMode next) {
	if (WALK_MOVES[int(w.mode)][int(next)]) w.mode = next;
}
