#!/usr/bin/env bash
# runq.sh <workspace> <evidence dir> <job...>: the verification as a queue of jobs, each watched while it runs.
# A job: "cmd:<name>:<shell command>", "test:<scene path>[@renderer]", "scene:<name>:<scene path>[:extra user args]", "scripts", "import", "gates", "build",
# "gates+build" (the gates, Python, run beside the two builds, each still -j 4: neither opens the game).
# Events go to <evidence>/events.log as they happen (one line each): INICIO, OK, FALHOU (exit 1 / FAIL lines), QUEBROU <code>
# (a crash or another code), TRAVOU (its log stopped growing for STALL_S seconds: the process is killed and the queue goes on),
# and every FAIL / FALHOU / SCRIPT ERROR line of a job the moment its log shows it. A script that does not parse breaks its job at once (the scene would never quit). No job has a deadline of its own: a job
# that keeps printing keeps running; one that goes quiet is the one that hung.
set -uo pipefail
WS="$1"; EV="$2"; shift 2
STALL_S="${STALL_S:-180}"
PY="C:/Users/Jonathan/AppData/Local/Programs/Python/Python312/python.exe"
GODOT="C:/Users/Jonathan/AppData/Local/Microsoft/WinGet/Links/godot_console.exe"
HIDDEN="$(cygpath -w "$WS/tools/build/hidden_run.ps1")"
HERE="$(cd "$(dirname "$0")" && pwd)"
WSW="$(cygpath -m "$WS")"
mkdir -p "$EV"
EVW="$(cygpath -m "$EV")"
EVENTS="$EV/events.log"
: > "$EVENTS"
say() { echo "$(date +%H:%M:%S) $*" >> "$EVENTS"; }

source "$HERE/runq_jobs.sh"

for job in "$@"; do
	case "$job" in
		gates) run_gates ;;
		build) run_builds ;;
		# the iteration loop: the scripts parsed first (seconds; the game holds the DLL, so not beside the link), then only the
		# debug DLL, the one tests load, on every core; the release DLL is built for the commit
		fast)
			godot_job scripts --headless --path project -s "$(cygpath -m "$HERE/check_scripts.gd")" || exit 1
			build_one debug || exit 1
			say "warnings: $(grep -cE 'warning:|error:' "$EV/build-debug.txt")" ;;
		gates+build)
			run_gates &
			gpid=$!
			run_builds
			wait "$gpid" ;;
		import) godot_job import --path project --headless --import ;;
		scripts) godot_job scripts --headless --path project -s "$(cygpath -m "$HERE/check_scripts.gd")" ;;
		test:*)
			spec="${job#test:}"; scene="${spec%@*}"; rend="gl_compatibility"
			[ "$spec" != "$scene" ] && rend="${spec#*@}"
			godot_job "$(basename "$scene")_$rend" --path project --rendering-method "$rend" "res://$scene.tscn" -- --no-flow ;;
		scene:*)
			# scene:<name>:<scene path>[:[@WxH ]<user args>] (the rest of the line is the args, colons and all)
			IFS=':' read -r _ name scene extra <<< "$job"
			res="1600x900"
			if [[ "${extra:-}" == @* ]]; then res="${extra%% *}"; res="${res#@}"; extra="${extra#* }"; [ "$extra" == "@$res" ] && extra=""; fi
			# shellcheck disable=SC2086
			godot_job "$name" --path project --resolution "$res" "res://$scene" -- --no-flow --capture-dir="$EVW/$name" ${extra:-} ;;
		cmd:*)
			IFS=':' read -r _ name command <<< "$job"
			watch_job "$name" "$EV/$name.txt" bash -c "{ $command; } > \"$EV/$name.txt\" 2>&1" ;;
		*) say "UNKNOWN JOB $job" ;;
	esac
done
say "END OF QUEUE"
