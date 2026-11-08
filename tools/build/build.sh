#!/usr/bin/env bash
# Build, test and package commands (documented in CLAUDE.md > Comandos). Usage: tools/build/build.sh <target>. Windows here; the Android library and APK in android.sh.
# Env: GODOT (Godot 4.7 binary), MINGW (llvm-mingw root, Windows cross-build), OUT (package folder). The Windows build uses
# Ninja: on a Windows host CMake would otherwise pick NMake, which is not installed.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
GODOT="$("${PY:-python}" "$ROOT/tools/tasks/cli.py" var godot)" # suites.json vars: GODOT first, then this machine's
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

export_all() {
	mkdir -p "$OUT/windows"
	"$GODOT" --headless --path "$ROOT/project" --export-release "Windows" "$OUT/windows/Roadcraft.exe"
}

readme() {
	cat > "$OUT/READ-ME.txt" <<'TXT'
Roadcraft - city builder (Godot 4.7 + nucleo C++)
Windows 10/11 64 bits: abra jogo/Roadcraft.exe
Camera (como no Roadcraft): botao direito arrastado gira/inclina; botao do meio (ou Shift+direito) arrasta o mapa;
roda = zoom no cursor; WASD/setas andam; Q/E giram; PgUp/PgDn inclinam; Shift acelera; Home volta.
Clique direito sem arrastar ou Esc cancela a ferramenta. Ctrl+S salva, Ctrl+O abre, Ctrl+Z desfaz.
Vias: clique no inicio e no fim (V alterna curva). Zonas/terreno: segure o botao esquerdo.
Fonte: pasta fonte/ (leia fonte/CLAUDE.md: regras, comandos e mapa do codigo).
TXT
}

package() {
	readme
	local stage="$OUT/stage"
	rm -rf "$stage" "$OUT/Roadcraft.zip" && mkdir -p "$stage/jogo" "$stage/fonte"
	cp -r "$OUT/windows/." "$stage/jogo/" && cp "$OUT/READ-ME.txt" "$stage/"
	git -C "$ROOT" archive --format=tar HEAD | tar -x -C "$stage/fonte" # the source as committed, in one pass
	(cd "$stage" && python3 -c "import shutil; shutil.make_archive('../Roadcraft', 'zip', '.')") # a tar with a .zip name is what Git Bash's tar.exe -a writes
	rm -rf "$stage"
	echo "$OUT/Roadcraft.zip"
}

case "${1:-}" in
	core) core_linux ;; # so para testes/capturas neste ambiente; o jogo e Windows
	core-windows) core_windows ;;
	dll-debug) dll template_debug "$ROOT/native/build-win-debug" ;;
	dll-release) dll template_release "$ROOT/native/build-win" ;;
	gates) gates ;;
	e2e) e2e ;;
	capture) capture "${2:-}" ;;
	export) export_all ;;
	package) package ;;
	all) core_linux; core_windows; gates; e2e; export_all; package ;;
	*) echo "targets: core | core-windows | dll-debug | dll-release | gates | e2e | capture [dir] | export | package | all"; exit 2 ;;
esac
