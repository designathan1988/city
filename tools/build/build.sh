#!/usr/bin/env bash
# The core's two Windows DLLs. dll-debug and dll-release build one DLL incrementally (its build folder configured the first
# time): they are the processes of the tasks build_debug and build_release of tools/tasks/suites.json. core-windows is the one
# way to build both: the orchestrator runs those two tasks, each with its deadline, its lines shown as they come, the dll lock
# and the cache. Usage: tools/build/build.sh core-windows | dll-debug | dll-release. Env: MINGW (llvm-mingw root), JOBS.
# The build uses Ninja: on a Windows host CMake would otherwise pick NMake, which is not installed.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 2)}"

# One DLL, incrementally: template_release ships in the export; template_debug is what the editor, tests and captures load.
dll() { # <template_debug|template_release> <build dir>
	bash "$ROOT/tools/build/free_dll.sh"
	[ -f "$2/build.ninja" ] || cmake -S "$ROOT/native" -B "$2" -G Ninja -DCMAKE_BUILD_TYPE=Release -DGODOTCPP_TARGET="$1" \
		-DCMAKE_TOOLCHAIN_FILE="$ROOT/native/toolchain-mingw.cmake" || return 1
	cmake --build "$2" --target citycore -j "$JOBS"
}

# The block rule (CLAUDE.md, Method step 5): nothing is built while a block's code is written. tools/tasks/block.py gives the
# one answer; core-windows asks it through `cli.py run`, which refuses a run while a block is open.
case "${1:-}" in
	dll-debug|dll-release) "${PY:-python}" "$ROOT/tools/tasks/block.py" check "build $1" || exit 1 ;;
esac

case "${1:-}" in
	core-windows) exec "${PY:-python}" "$ROOT/tools/tasks/cli.py" run build_debug build_release ;;
	dll-debug) dll template_debug "$ROOT/native/build-win-debug" ;;
	dll-release) dll template_release "$ROOT/native/build-win" ;;
	*) echo "targets: core-windows | dll-debug | dll-release" >&2; exit 2 ;;
esac
