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
	for target in template_release template_debug; do # at once: the link of one runs beside the compile of the other
		dir="$ROOT/native/build-win"; [ "$target" = template_debug ] && dir="$dir-debug"
		(cmake -S "$ROOT/native" -B "$dir" -G Ninja -DCMAKE_BUILD_TYPE=Release -DGODOTCPP_TARGET="$target" \
			-DCMAKE_TOOLCHAIN_FILE="$ROOT/native/toolchain-mingw.cmake" && cmake --build "$dir" --target citycore -j "$JOBS") &
	done
	for target in $(jobs -p); do wait "$target" || failed=1; done
	return $failed
}

gates() { python3 "$ROOT/tools/gates/run_gates.py"; }

run_with_display() {
	if command -v xvfb-run >/dev/null 2>&1; then
		xvfb-run -a -s "-screen 0 1280x720x24" "$@"
	else
		"$@"
	fi
}

e2e() { # every test scene, each with a time limit (tools/build/run_tests.sh)
	"$GODOT" --headless --path "$ROOT/project" --import >/dev/null 2>&1 || true
	local wrap=(); command -v xvfb-run >/dev/null 2>&1 && wrap=(xvfb-run -a -s "-screen 0 1280x720x24")
	bash "$ROOT/tools/build/run_tests.sh" "$GODOT" "$ROOT/project" "$OUT/tests" "${wrap[@]}"
}

capture() {
	local dir="${1:-$OUT/captures}"
	mkdir -p "$dir"
	run_with_display "$GODOT" --path "$ROOT/project" --resolution 1600x900 res://tests/visual/capture.tscn -- --capture-dir="$dir"
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
	gates) gates ;;
	e2e) e2e ;;
	capture) capture "${2:-}" ;;
	export) export_all ;;
	package) package ;;
	all) core_linux; core_windows; gates; e2e; export_all; package ;;
	*) echo "alvos: core | core-windows | gates | e2e | capture [dir] | export | package | all"; exit 1 ;;
esac
