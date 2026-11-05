#!/usr/bin/env bash
# quick.sh [--all] [scene,scene...]: what a change asks for, in one command and at once. The scripts compile beside the debug DLL
# while the gates of the changed files run; then the scenes the change touches and the edit fuzz run in parallel.
# --all also rebuilds both DLLs and runs every scene: the check before a commit. Each step prints its seconds; any failure ends in exit 1.
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT" || exit 2
GODOT="C:/Users/Jonathan/AppData/Local/Microsoft/WinGet/Links/godot_console.exe"
export MINGW="${MINGW:-C:/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64}"
PY="C:/Users/Jonathan/AppData/Local/Programs/Python/Python312/python.exe"
OUT="$ROOT/dist/quick"
mkdir -p "$OUT"
started=$SECONDS
all=""; picked=""
for a in "$@"; do [ "$a" = --all ] && all=1 || picked="$a"; done
changed="$(git status --short | awk '{print $2}')"

# The scenes a path touches; a path of native/ or project/ with no row here runs every scene.
scenes_of() {
	case "$1" in
		native/src/sim/roads/*|native/src/sim/commands/road*|native/src/render/roads/*|project/game/tools/road*) echo road_commands_test road_edit_test road_grade_test ;;
		native/src/sim/commands/street*|native/src/sim/street/*|native/src/render/props/*) echo street_works_test ;;
		native/src/sim/traffic/ped*|native/src/render/agents/people*) echo people_test ;;
		native/src/sim/traffic/*) echo yard_test ;;
		native/src/sim/transit/*|native/src/render/transit/*) echo transit_test ;;
		native/src/sim/buildings/interior/*|native/src/render/buildings/interior*) echo interior_test ;;
		native/src/sim/buildings/*|native/src/sim/commands/body*|native/src/render/buildings/*) echo building_edit_test building_tool_test model_test ;;
		native/src/sim/persistence/*|native/src/core/bytes.h) echo save_undo_test ;;
		project/ui/*) echo shell_test ;;
		project/game/camera/*) echo camera_input_test ;;
		log/*|tools/*|CLAUDE.md|*.md) ;;
		*) echo ALL ;;
	esac
}

scenes="edit_fuzz_test"
for f in $changed; do scenes="$scenes $(scenes_of "$f")"; done
[ -n "$picked" ] && scenes="$scenes ${picked//,/ }"
scenes="$(echo "$scenes" | tr ' ' '\n' | sort -u | tr '\n' ',' | sed 's/,$//; s/^,//')"
case ",$scenes," in *,ALL,*) all=1 ;; esac

bash tools/build/gates_changed.sh "$ROOT" > "$OUT/gates.txt" 2>&1 & gates=$!
"$GODOT" --headless --path project -s "$(cygpath -m "$ROOT/tools/build/check_scripts.gd")" > "$OUT/scripts.txt" 2>&1 & scripts=$!
if [ -n "$all" ]; then
	bash tools/build/build.sh core-windows > "$OUT/build.txt" 2>&1
else
	cmake --build native/build-win-debug --target citycore > "$OUT/build.txt" 2>&1
fi
built=$?
wait $scripts; scripted=$?
echo "build $built, scripts $scripted: $((SECONDS - started)) s"
[ "$built" = 0 ] || { grep -m12 "error:" "$OUT/build.txt"; exit 1; }
[ "$scripted" = 0 ] || { grep -m12 "ERROR\|FALHA" "$OUT/scripts.txt"; exit 1; }

if [ -n "$all" ]; then only=""; else only="--only=$scenes"; fi
"$PY" tools/analysis/suite.py --parallel 8 --godot "$GODOT" $only > "$OUT/suite.txt" 2>&1
suite=$?
wait $gates; gated=$?
tail -1 "$OUT/suite.txt"
tail -1 "$OUT/gates.txt"
echo "total $((SECONDS - started)) s"
[ "$suite" = 0 ] && [ "$gated" = 0 ]
