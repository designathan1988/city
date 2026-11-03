#!/usr/bin/env bash
# Build, test and package commands (documented in CLAUDE.md > Comandos). Usage: tools/build/build.sh <target>. Game ships for Windows only.
# Env: GODOT (Godot 4.7 binary), MINGW (llvm-mingw root, Windows cross-build), OUT (package folder).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
GODOT="${GODOT:-godot}"
OUT="${OUT:-$ROOT/dist}"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 2)}"

core_linux() {
	cmake -S "$ROOT/native" -B "$ROOT/native/build-linux" -DCMAKE_BUILD_TYPE=Release
	cmake --build "$ROOT/native/build-linux" --target citycore -j "$JOBS"
}

core_windows() {
	cmake -S "$ROOT/native" -B "$ROOT/native/build-win" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="$ROOT/native/toolchain-mingw.cmake"
	cmake --build "$ROOT/native/build-win" --target citycore -j "$JOBS"
}

gates() { python3 "$ROOT/tools/gates/run_gates.py"; }

run_with_display() {
	if command -v xvfb-run >/dev/null 2>&1; then
		xvfb-run -a -s "-screen 0 1280x720x24" "$@"
	else
		"$@"
	fi
}

e2e() {
	"$GODOT" --headless --path "$ROOT/project" --import >/dev/null 2>&1 || true
	run_with_display "$GODOT" --path "$ROOT/project" --rendering-method gl_compatibility \
		res://tests/e2e/e2e.tscn -- --no-flow
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
	cat > "$OUT/LEIA-ME.txt" <<'TXT'
Roadcraft - city builder (Godot 4.7 + nucleo C++)
Windows 10/11 64 bits: abra jogo/Roadcraft.exe
Camera (como no Roadcraft): botao direito arrastado gira/inclina; botao do meio (ou Shift+direito) arrasta o mapa;
roda = zoom no cursor; WASD/setas andam; Q/E giram; PgUp/PgDn inclinam; Shift acelera; Home volta.
Clique direito sem arrastar ou Esc cancela a ferramenta; Esc abre o menu de pausa. Ctrl+S salva, Ctrl+O abre, Ctrl+Z desfaz.
Vias: clique no inicio e no fim (C alterna curva). Zonas/terreno: segure o botao esquerdo.
Fonte: pasta fonte/ (leia fonte/CLAUDE.md: regras, comandos e mapa do codigo).
TXT
}

package() {
	readme
	local stage="$OUT/stage"
	rm -rf "$stage" "$OUT/Roadcraft.zip" && mkdir -p "$stage/jogo" "$stage/fonte"
	cp -r "$OUT/windows/." "$stage/jogo/" && cp "$OUT/LEIA-ME.txt" "$stage/"
	(cd "$ROOT" && git ls-files 2>/dev/null || find CLAUDE.md .claude native/src native/CMakeLists.txt native/toolchain-mingw.cmake \
		project tools -type f ! -path "*/.godot/*" ! -path "*/__pycache__/*") | while read -r f; do
		mkdir -p "$stage/fonte/$(dirname "$f")" && cp "$ROOT/$f" "$stage/fonte/$f"
	done
	(cd "$stage" && if command -v zip >/dev/null 2>&1; then zip -qr "$OUT/Roadcraft.zip" .; else tar.exe -a -cf "$OUT/Roadcraft.zip" .; fi)
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
