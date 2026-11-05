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

# Stops this job's own game and nothing else (other sessions run theirs on this machine): the hidden_run whose command writes this
# job's log, and the tree under it (cmd, the game), by its PID, as hidden_run stops it at its own time-out.
kill_job() { # <log>
	local key; key=$(cygpath -m "$1")
	powershell -NoProfile -Command "Get-CimInstance Win32_Process -Filter \"Name='powershell.exe'\" | Where-Object { \$_.CommandLine -like '*$key*' } | ForEach-Object { & taskkill.exe /PID \$_.ProcessId /T /F }" > /dev/null 2>&1
}

# Runs `cmd` in the background writing `log`; follows the log: its FAIL lines are events, a quiet log past STALL_S is a hang.
watch_job() { # <name> <log> <cmd...>
	local name="$1" log="$2"; shift 2
	: > "$log"
	say "INICIO $name"
	"$@" &
	local pid=$! size=-1 still=0 seen=0
	while kill -0 "$pid" 2> /dev/null; do
		sleep 2
		local now; now=$(stat -c %s "$log" 2> /dev/null || echo 0)
		if [ "$now" -ne "$size" ]; then
			size=$now; still=0
			local lines; lines=$(grep -nE "FAIL|FALHOU|SCRIPT ERROR|ERROR:.*(Crash|crash)" "$log" 2> /dev/null | tail -n +$((seen + 1)))
			if [ -n "$lines" ]; then
				while IFS= read -r l; do say "  $name: ${l#*:}"; seen=$((seen + 1)); done <<< "$lines"
			fi
			# a scene whose script did not parse never quits: it is broken now, not after STALL_S of silence (the scripts
			# job lists every parse error and quits by itself)
			if [ "$name" != scripts ] && grep -qE "Failed to load script .*Parse error|SCRIPT ERROR: Parse Error" "$log" 2> /dev/null; then
				say "QUEBROU $name (script não carregou: $(grep -m1 -oE 'Parse Error: .*' "$log" | cut -c1-160))"
				kill_job "$log"; kill "$pid" 2> /dev/null; wait "$pid" 2> /dev/null
				return 4
			fi
		else
			still=$((still + 2))
			if [ "$still" -ge "$STALL_S" ]; then
				say "TRAVOU $name (log parado há ${STALL_S}s; última linha: $(tail -n 1 "$log" | cut -c1-160))"
				kill_job "$log"; kill "$pid" 2> /dev/null; wait "$pid" 2> /dev/null
				return 3
			fi
		fi
	done
	wait "$pid"; local code=$?
	# a job through hidden_run.ps1 ends with "FIM <code>" in <log>.run: the game's own code (a Windows code does not fit bash's 0-255)
	if [ -f "$log.run" ]; then
		local fim; fim=$(grep -oE 'FIM -?[0-9]+' "$log.run" | tail -n 1 | awk '{print $2}')
		[ -n "$fim" ] && code=$fim
	fi
	local err; err=$(grep -c "SCRIPT ERROR" "$log" 2> /dev/null || true)
	if [ "$code" -eq 0 ] && [ "${err:-0}" -eq 0 ]; then say "OK $name"
	elif [ "$code" -eq 1 ] || [ "${err:-0}" -gt 0 ]; then say "FALHOU $name (saída $code, $err SCRIPT ERROR)"
	elif [ "$code" -eq 3221225477 ]; then say "QUEBROU $name (violação de acesso 0xC0000005; última linha: $(tail -n 1 "$log" | cut -c1-160))"
	else say "QUEBROU $name (saída $code; última linha: $(tail -n 1 "$log" | cut -c1-160))"
	fi
	[ "$code" -ge 0 ] && [ "$code" -le 255 ] && return "$code"
	return 255
}

godot_job() { # <name> <godot args...>: through hidden_run (no window), no deadline of its own (the watch decides)
	local name="$1"; shift
	local log="$EV/$name.txt"
	local args="" a
	for a in "$@"; do args+=" \"$a\""; done
	rm -f "$log.run"
	watch_job "$name" "$log" bash -c 'powershell -NoProfile -ExecutionPolicy Bypass -File "$0" -TimeoutS 86400 -WorkDir "$1" -Command "$2" > "$3" 2>&1' \
		"$HIDDEN" "$WSW" "\"$GODOT\"$args > \"$EVW/$name.txt\" 2>&1" "$log.run"
}

run_gates() { watch_job gates "$EV/gates.txt" bash -c "\"$PY\" tools/gates/run_gates.py > \"$EV/gates.txt\" 2>&1"; }
J="${BUILD_J:-$(nproc)}" # every core: ninja keeps them busy, the build is the long pole
build_one() { # <release|debug>
	local d=native/build-win; [ "$1" = debug ] && d="$d-debug"
	watch_job "build-$1" "$EV/build-$1.txt" bash -c "cmake --build $d -j $J > \"$EV/build-$1.txt\" 2>&1"
}
run_builds() {
	build_one release
	build_one debug
	say "avisos: $(cat "$EV"/build-*.txt | grep -cE 'warning:|error:')"
}

cd "$WS" || exit 2
for job in "$@"; do
	case "$job" in
		gates) run_gates ;;
		build) run_builds ;;
		# the iteration loop: the scripts parsed first (seconds; the game holds the DLL, so not beside the link), then only the
		# debug DLL, the one tests load, on every core; the release DLL is built for the commit
		fast)
			godot_job scripts --headless --path project -s "$(cygpath -m "$HERE/check_scripts.gd")" || exit 1
			build_one debug || exit 1
			say "avisos: $(grep -cE 'warning:|error:' "$EV/build-debug.txt")" ;;
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
		*) say "TAREFA DESCONHECIDA $job" ;;
	esac
done
say "FIM DA FILA"
