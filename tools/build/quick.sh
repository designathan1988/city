#!/usr/bin/env bash
# quick.sh [--all] [scene,scene...]: what a change asks for, in one command and at once. The scripts compile beside the debug DLL
# while the gates of the changed files run; then the scenes the change touches and the edit fuzz run in parallel.
# --all also rebuilds both DLLs and runs every scene. --ready rebuilds both DLLs and runs the same short check: the check before a commit;
# when all of it holds it leaves dist/quick/passed, newer than the code. Each step prints its seconds; any failure ends in exit 1.
# Beside the scenes, a fixed city is hashed (open_probe --digest) and held to the golden in dist/quick: an optimisation leaves it
# equal, with no dump and no photograph; --bless says the change means to move it. The still world is photographed frozen (look
# --frozen: no clock, no TIME, waits by frames) against its golden too: bit for bit when nothing moved, past 0.1 % it is told.
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT" || exit 2
GODOT="C:/Users/Jonathan/AppData/Local/Microsoft/WinGet/Links/godot_console.exe"
export MINGW="${MINGW:-C:/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64}"
PY="C:/Users/Jonathan/AppData/Local/Programs/Python/Python312/python.exe"
OUT="$ROOT/dist/quick"
mkdir -p "$OUT"
started=$SECONDS
all=""; picked=""; bless=""; ready=""
for a in "$@"; do
	case "$a" in --all) all=1 ;; --bless) bless=1 ;; --ready) ready=1 ;; *) picked="$a" ;; esac
done
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
		# What only changes the picture: the frozen photographs below look at it, with no scene. A DLL is built from sources that have their row.
		project/bin/*|project/render/*|project/data/render/*|project/tests/visual/*) ;;
		*) echo ALL ;;
	esac
}

scenes="edit_fuzz_test traffic_hash_test"
for f in $changed; do scenes="$scenes $(scenes_of "$f")"; done
[ -n "$picked" ] && scenes="$scenes ${picked//,/ }"
scenes="$(echo "$scenes" | tr ' ' '\n' | sort -u | tr '\n' ',' | sed 's/,$//; s/^,//')"
case ",$scenes," in *,ALL,*) all=1 ;; esac

bash tools/build/gates_changed.sh "$ROOT" > "$OUT/gates.txt" 2>&1 & gates=$!
# A class new to the project, written here or merged, is known only after an import.
"$GODOT" --headless --path project --import > "$OUT/import.txt" 2>&1
"$GODOT" --headless --path project -s "$(cygpath -m "$ROOT/tools/build/check_scripts.gd")" > "$OUT/scripts.txt" 2>&1 & scripts=$!
if [ -n "$all$ready" ]; then
	bash tools/build/build.sh core-windows > "$OUT/build.txt" 2>&1
else
	cmake --build native/build-win-debug --target citycore > "$OUT/build.txt" 2>&1
fi
built=$?
wait $scripts; scripted=$?
echo "build $built, scripts $scripted: $((SECONDS - started)) s"
[ "$built" = 0 ] || { grep -m12 "error:" "$OUT/build.txt"; exit 1; }
[ "$scripted" = 0 ] || { grep -m12 "ERROR\|FALHA" "$OUT/scripts.txt"; exit 1; }

"$GODOT" --headless --path project res://tests/perf/open_probe.tscn -- --no-flow --digest 2>&1 | grep -a "^DIGEST" > "$OUT/digest.txt" & digest=$!
rm -rf "$OUT/photos"; mkdir -p "$OUT/photos"
WIN="$(cygpath -m "$ROOT")"
powershell -NoProfile -ExecutionPolicy Bypass -File tools/build/hidden_run.ps1 -Command "\"$GODOT\" --path \"$WIN/project\" --resolution 1280x720 res://tests/visual/look.tscn -- --no-flow --frozen --plan=res://tests/visual/frozen_plan.json --capture-dir=$WIN/dist/quick/photos > \"$WIN/dist/quick/photos.txt\" 2>&1" -TimeoutS 90 -WorkDir . > /dev/null & photos=$!
if [ -n "$all" ]; then only=""; else only="--only=$scenes"; fi
"$PY" tools/analysis/suite.py --parallel 8 --godot "$GODOT" $only > "$OUT/suite.txt" 2>&1
suite=$?
wait $gates; gated=$?
wait $digest
same=0
if [ ! -s "$OUT/digest.txt" ]; then echo "digest: the probe gave none"; same=1
elif [ -n "$bless" ] || [ ! -f "$OUT/golden.txt" ]; then cp "$OUT/digest.txt" "$OUT/golden.txt"; echo "digest: golden written"
elif cmp -s "$OUT/digest.txt" "$OUT/golden.txt"; then echo "digest: equal to the golden"
else echo "digest MOVED:"; diff "$OUT/golden.txt" "$OUT/digest.txt"; same=1
fi
tail -1 "$OUT/suite.txt"
tail -1 "$OUT/gates.txt"
echo "total $((SECONDS - started)) s"
wait $photos
if [ -n "$bless" ] || [ ! -d "$OUT/golden_photos" ]; then rm -rf "$OUT/golden_photos"; cp -r "$OUT/photos" "$OUT/golden_photos"; echo "photos: golden written"
else "$PY" tools/build/photodiff.py "$OUT/golden_photos" "$OUT/photos" "$OUT/photos_diff" 0.1 || same=1
fi
rm -f "$OUT/passed"
[ "$suite" = 0 ] && [ "$gated" = 0 ] && [ "$same" = 0 ] || exit 1
[ -n "$ready" ] && touch "$OUT/passed"
exit 0
