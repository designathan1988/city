#!/usr/bin/env bash
# Delivers the game to the owner: the Windows export, run by the orchestrator (the task export of tools/tasks/suites.json: its
# deadline, its lines shown as they come, the dll lock, after the release DLL and the import), moved to a folder of its own,
# and the Desktop's Roadcraft shortcut pointed at it. Usage: tools/build/deliver.sh export.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILDS="$ROOT/dist/builds"
NEXT="$BUILDS/next" # the export task's declared output

# Every export ends in dist/builds/<date>_<commit>[-dirty]: a game the owner left open holds its files (Windows refuses to
# replace a running exe and the pack it reads), so the next export never waits on it. dist/builds/latest.txt names the newest.
export_game() {
	local name dir
	rm -rf "$NEXT" # an absolute path under dist/builds, this tool's own folder
	mkdir -p "$NEXT"
	"${PY:-python}" "$ROOT/tools/tasks/cli.py" run export || { echo "export failed: the task export did not pass" >&2; return 1; }
	[ -s "$NEXT/Roadcraft.exe" ] && [ -s "$NEXT/Roadcraft.pck" ] || { echo "export failed: $NEXT has no Roadcraft.exe and .pck" >&2; return 1; }
	name="$(date +%Y-%m-%d_%H%M)_$(git -C "$ROOT" rev-parse --short HEAD)"
	git -C "$ROOT" diff --quiet HEAD -- project || name="$name-dirty"
	[ -e "$BUILDS/$name" ] && name="${name}_$(date +%S)" # a second export in the same minute: mv would nest it in the first
	dir="$BUILDS/$name"
	mv "$NEXT" "$dir"
	printf '%s\n' "$name" > "$BUILDS/latest.txt"
	desktop_shortcut "$dir/Roadcraft.exe"
	# the builds' bound (tools/tasks/suites.json lifecycle: the last two, and any build an open problem's evidence names)
	"${PY:-python}" "$ROOT/tools/tasks/cli.py" clean --only dist/builds
	echo "$dir/Roadcraft.exe"
}

desktop_shortcut() { # <exe>: the Desktop's Roadcraft shortcut opens it (Windows hosts only)
	command -v powershell >/dev/null || return 0
	local exe
	exe="$(cygpath -w "$1")"
	powershell -NoProfile -Command "\$s = (New-Object -ComObject WScript.Shell).CreateShortcut([IO.Path]::Combine([Environment]::GetFolderPath('Desktop'), 'Roadcraft.lnk')); \$s.TargetPath = '$exe'; \$s.WorkingDirectory = [IO.Path]::GetDirectoryName('$exe'); \$s.Save()"
}

# The block rule (CLAUDE.md, Method step 5): no export while a block's code is written (tools/tasks/block.py answers).
"${PY:-python}" "$ROOT/tools/tasks/block.py" check "export ${1:-}" || exit 1

case "${1:-}" in
	export) export_game ;;
	*) echo "targets: export" >&2; exit 2 ;;
esac
