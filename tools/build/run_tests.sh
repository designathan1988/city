#!/usr/bin/env bash
# Runs every test scene (core + e2e) through the layer the player sees (tools/build/build.sh e2e), on Compatibility and, for
# what the screen shows, on Forward+ too. A test fails when it exits non-zero, hangs past TEST_TIMEOUT_S (a script that
# failed to load never quits) or logs a SCRIPT ERROR.
# Usage: run_tests.sh <godot binary> <project dir> <log dir> [wrapper...] (the wrapper is xvfb-run when there is one).
set -uo pipefail
GODOT="$1"; PROJECT="$2"; LOGS="$3"; shift 3
TEST_TIMEOUT_S="${TEST_TIMEOUT_S:-600}"
TESTS=(tests/core/strike_test tests/core/shoot_test tests/core/road_commands_test tests/core/save_undo_test tests/core/camera_input_test tests/e2e/e2e tests/core/light_color_test tests/core/building_light_test)
BOTH_RENDERERS=(tests/core/light_color_test tests/core/building_light_test) # what the screen shows: also on Forward+, the game's renderer
mkdir -p "$LOGS"
failed=0
run_one() { # <test> <rendering method>
	local log="$LOGS/$(basename "$1")_$2.log"
	"${WRAP[@]}" timeout "$TEST_TIMEOUT_S" "$GODOT" --path "$PROJECT" --rendering-method "$2" "res://$1.tscn" -- --no-flow >"$log" 2>&1
	local code=$?
	if [ "$code" -ne 0 ] || grep -q "SCRIPT ERROR" "$log"; then
		echo "FALHOU $1 em $2 (saída $code): $log"
		grep -E "FAIL|SCRIPT ERROR" "$log" | head -5
		failed=1
	else
		echo "ok $1 ($2)"
	fi
}
WRAP=("$@")
for t in "${TESTS[@]}"; do
	run_one "$t" gl_compatibility
	for b in "${BOTH_RENDERERS[@]}"; do [ "$b" = "$t" ] && run_one "$t" forward_plus; done
done
exit "$failed"
