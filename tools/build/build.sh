#!/usr/bin/env bash
# Build, test and package commands (documented in CLAUDE.md > Comandos). Usage: tools/build/build.sh <target>. Game ships for Windows only.
# Env: GODOT (Godot 4.7 binary), MINGW (llvm-mingw root, Windows cross-build), OUT (package folder). The Windows build uses
# Ninja: on a Windows host CMake would otherwise pick NMake, which is not installed.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
GODOT="${GODOT:-godot}"
OUT="${OUT:-$ROOT/dist}"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 2)}"

core_linux() {
	cmake -S "$ROOT/native" -B "$ROOT/native/build-linux" -DCMAKE_BUILD_TYPE=Release
	cmake --build "$ROOT/native/build-linux" --target citycore -j "$JOBS"
}

core_windows() { # template_release ships in the export; template_debug is what the editor, tests and captures load
	local dir target failed=0
	bash "$ROOT/tools/build/free_dll.sh"
	for target in template_release template_debug; do # at once: the link of one runs beside the compile of the other
		dir="$ROOT/native/build-win"; [ "$target" = template_debug ] && dir="$dir-debug"
		(cmake -S "$ROOT/native" -B "$dir" -G Ninja -DCMAKE_BUILD_TYPE=Release -DGODOTCPP_TARGET="$target" \
			-DCMAKE_TOOLCHAIN_FILE="$ROOT/native/toolchain-mingw.cmake" && cmake --build "$dir" --target citycore -j "$JOBS") &
	done
	for target in $(jobs -p); do wait "$target" || failed=1; done
	return $failed
}

# One DLL, incrementally (the build folder is configured the first time): the build tasks of tools/tasks/suites.json.
dll() { # <template_debug|template_release> <build dir>
	bash "$ROOT/tools/build/free_dll.sh"
	[ -f "$2/build.ninja" ] || cmake -S "$ROOT/native" -B "$2" -G Ninja -DCMAKE_BUILD_TYPE=Release -DGODOTCPP_TARGET="$1" \
		-DCMAKE_TOOLCHAIN_FILE="$ROOT/native/toolchain-mingw.cmake" || return 1
	cmake --build "$2" --target citycore -j "$JOBS"
}

gates() { python3 "$ROOT/tools/gates/run_gates.py"; }

# Every test scene and the captures are suites of tools/tasks: each scene a task with its own deadline, on the hidden desktop.
e2e() { "${PY:-python}" "$ROOT/tools/tasks/cli.py" run tests; }

capture() { # [dir]: one folder per capture task under it
	CAPTURE_DIR="${1:-$OUT/captures}" "${PY:-python}" "$ROOT/tools/tasks/cli.py" run capture
}

deliver() { bash "$ROOT/tools/build/deliver.sh" "$@"; } # the export, the Desktop's shortcut and the zip

case "${1:-}" in
	core) core_linux ;; # so para testes/capturas neste ambiente; o jogo e Windows
	core-windows) core_windows ;;
	dll-debug) dll template_debug "$ROOT/native/build-win-debug" ;;
	dll-release) dll template_release "$ROOT/native/build-win" ;;
	gates) gates ;;
	e2e) e2e ;;
	capture) capture "${2:-}" ;;
	export) deliver export ;;
	package) deliver package ;;
	all) core_linux; core_windows; gates; e2e; deliver export; deliver package ;;
	*) echo "targets: core | core-windows | dll-debug | dll-release | gates | e2e | capture [dir] | export | package | all"; exit 2 ;;
esac
