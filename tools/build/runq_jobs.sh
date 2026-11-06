# How runq.sh runs a job: watched by its log, stopped by its own PID, the game on the hidden desktop. Sourced by runq.sh, with its variables.
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
	say "START $name"
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
				say "BROKE $name (script did not load: $(grep -m1 -oE 'Parse Error: .*' "$log" | cut -c1-160))"
				kill_job "$log"; kill "$pid" 2> /dev/null; wait "$pid" 2> /dev/null
				return 4
			fi
		else
			still=$((still + 2))
			if [ "$still" -ge "$STALL_S" ]; then
				say "HUNG $name (log still for ${STALL_S}s; last line: $(tail -n 1 "$log" | cut -c1-160))"
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
	elif [ "$code" -eq 1 ] || [ "${err:-0}" -gt 0 ]; then say "FAILED $name (exit $code, $err SCRIPT ERROR)"
	elif [ "$code" -eq 3221225477 ]; then say "BROKE $name (access violation 0xC0000005; last line: $(tail -n 1 "$log" | cut -c1-160))"
	else say "BROKE $name (exit $code; last line: $(tail -n 1 "$log" | cut -c1-160))"
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
	say "warnings: $(cat "$EV"/build-*.txt | grep -cE 'warning:|error:')"
}

cd "$WS" || exit 2
