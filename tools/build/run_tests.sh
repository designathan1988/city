#!/usr/bin/env bash
# Runs every test scene (core + e2e) through the layer the player sees (tools/build/build.sh e2e). A test fails when
# it exits non-zero, hangs past TEST_TIMEOUT_S (a script that failed to load never quits) or logs a SCRIPT ERROR.
# Usage: run_tests.sh <godot binary> <project dir> <log dir> [wrapper...] (the wrapper is xvfb-run when there is one).
set -uo pipefail
GODOT="$1"; PROJECT="$2"; LOGS="$3"; shift 3
TEST_TIMEOUT_S="${TEST_TIMEOUT_S:-600}"
TESTS=(tests/core/road_commands_test tests/core/save_undo_test tests/core/camera_input_test tests/e2e/e2e)
mkdir -p "$LOGS"
failed=0
for t in "${TESTS[@]}"; do
	log="$LOGS/$(basename "$t").log"
	"$@" timeout "$TEST_TIMEOUT_S" "$GODOT" --path "$PROJECT" --rendering-method gl_compatibility "res://$t.tscn" -- --no-flow >"$log" 2>&1
	code=$?
	if [ "$code" -ne 0 ] || grep -q "SCRIPT ERROR" "$log"; then
		echo "FALHOU $t (saída $code): $log"
		grep -E "FAIL|SCRIPT ERROR" "$log" | head -5
		failed=1
	else
		echo "ok $t"
	fi
done
exit "$failed"
