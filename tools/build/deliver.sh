#!/usr/bin/env bash
# Delivers the game to the owner: the Windows export in a folder of its own, the Desktop's Roadcraft shortcut pointed at the
# newest one, and the zip of the newest export with the source as committed. Usage: tools/build/deliver.sh export | package.
# Env: GODOT (Godot 4.7 binary), OUT (the dist folder).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
GODOT="${GODOT:-godot}"
OUT="${OUT:-$ROOT/dist}"
BUILDS="$OUT/builds"

# Every export goes to dist/builds/<date>_<commit>[-dirty]: a game the owner left open holds its files (Windows refuses to
# replace a running exe and the pack it reads), so the next export never waits on it. dist/builds/latest.txt names the newest.
export_game() {
	local name dir
	name="$(date +%Y-%m-%d_%H%M)_$(git -C "$ROOT" rev-parse --short HEAD)"
	git -C "$ROOT" diff --quiet HEAD -- project || name="$name-dirty"
	dir="$BUILDS/$name"
	mkdir -p "$dir"
	"$GODOT" --headless --path "$ROOT/project" --export-release "Windows" "$dir/Roadcraft.exe"
	[ -s "$dir/Roadcraft.exe" ] && [ -s "$dir/Roadcraft.pck" ] || { echo "export failed: $dir has no Roadcraft.exe and .pck"; return 1; }
	printf '%s\n' "$name" > "$BUILDS/latest.txt"
	desktop_shortcut "$dir/Roadcraft.exe"
	echo "$dir/Roadcraft.exe"
}

desktop_shortcut() { # <exe>: the Desktop's Roadcraft shortcut opens it (Windows hosts only)
	command -v powershell >/dev/null || return 0
	local exe
	exe="$(cygpath -w "$1")"
	powershell -NoProfile -Command "\$s = (New-Object -ComObject WScript.Shell).CreateShortcut([IO.Path]::Combine([Environment]::GetFolderPath('Desktop'), 'Roadcraft.lnk')); \$s.TargetPath = '$exe'; \$s.WorkingDirectory = [IO.Path]::GetDirectoryName('$exe'); \$s.Save()"
}

newest() {
	[ -s "$BUILDS/latest.txt" ] || { echo "no export yet: run tools/build/deliver.sh export" >&2; return 1; }
	echo "$BUILDS/$(cat "$BUILDS/latest.txt")"
}

readme() {
	cat > "$OUT/READ-ME.txt" <<'TXT'
Roadcraft - city builder (Godot 4.7 with a C++ core)
Windows 10/11, 64 bits: open game/Roadcraft.exe
Camera: the right button dragged turns and tilts; the middle button (or Shift+right) drags the map; the wheel zooms at the
cursor; WASD or the arrows move; Q/E turn; PgUp/PgDn tilt; Shift speeds up; Home goes back.
A right click without a drag, or Esc, cancels the tool. Ctrl+S saves, Ctrl+O opens, Ctrl+Z undoes.
Roads: click the start and the end (V switches to a curve). Zones and terrain: hold the left button.
Source: the source/ folder (read source/CLAUDE.md: rules, commands and the map of the code).
TXT
}

package() {
	local built stage="$OUT/stage"
	built="$(newest)"
	readme
	rm -rf "$stage" "$OUT/Roadcraft.zip" && mkdir -p "$stage/game" "$stage/source"
	cp -r "$built/." "$stage/game/" && cp "$OUT/READ-ME.txt" "$stage/"
	git -C "$ROOT" archive --format=tar HEAD | tar -x -C "$stage/source" # the source as committed, in one pass
	(cd "$stage" && python3 -c "import shutil; shutil.make_archive('../Roadcraft', 'zip', '.')") # a tar with a .zip name is what Git Bash's tar.exe -a writes
	rm -rf "$stage"
	echo "$OUT/Roadcraft.zip"
}

case "${1:-}" in
	export) export_game ;;
	package) package ;;
	*) echo "targets: export | package"; exit 2 ;;
esac
