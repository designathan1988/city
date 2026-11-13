#!/usr/bin/env bash
# pgo.sh: the profile the release DLL is optimized by (native/pgo/citycore.profdata). An instrumented DLL, built as the export's, plays
# the game hidden: the camera over a running city, edits, roads, people, yards. The counts are merged and committed; a build then
# needs no training. Stale functions only lose their counts, so a new profile is a choice, not a duty: after a change of the hot paths.
# Every step has a deadline and shows its lines as they come (the configure and the build are also kept under dist/pgo/logs).
usage() { awk 'NR > 1 && !/^#/ { exit } NR > 1 { sub(/^# ?/, ""); print }' "$0"; echo "usage: bash tools/build/pgo.sh [-h|--help]"; }
case "${1:-}" in "") ;; -h|--help) usage; exit 0;; *) usage >&2; exit 2;; esac # help, or a wrong argument, never trains
set -o pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT" || exit 2
# The block rule (CLAUDE.md, Method step 5): nothing is built or run while a block's code is written (tools/tasks/block.py).
"${PY:-python}" tools/tasks/block.py check "pgo" || exit 1
# The machine's tools from their one owner (tools/tasks/locations.py: the environment, tools/tasks/local.json, the PATH)
MINGW="$("${PY:-python}" tools/tasks/locations.py mingw)" || exit 2
GODOT="$("${PY:-python}" tools/tasks/locations.py godot)" || exit 2
export MINGW
OUT="$ROOT/dist/pgo"
BUILD="$ROOT/dist/build-pgo"
MIRROR="$OUT/project"
SCENES="tests/perf/camera_pan tests/core/edit_fuzz_test tests/core/road_commands_test tests/core/people_test tests/core/yard_test"
DEADLINE_S=900 # each scene's run (a task of tools/tasks)
CONFIGURE_DEADLINE_S=300
BUILD_DEADLINE_S=3600 # the instrumented DLL from nothing: the deadline of the build tasks of tools/tasks/suites.json
MERGE_DEADLINE_S=300
win() { cygpath -m "$1"; }

# The mirror's junctions lead into the project: rmdir takes a junction away and never what it points to. Run on every exit and
# before anything deletes dist/pgo, so a junction an earlier failed run left can never let rm -rf delete into the project.
unlink_mirror() {
	local entry left=0
	for entry in "$MIRROR"/* "$MIRROR/.godot"; do
		[ -d "$entry" ] || [ -L "$entry" ] || continue
		cmd //c rmdir "$(cygpath -w "$entry")" > /dev/null 2>&1
		[ -L "$entry" ] && { echo "pgo: the junction $entry is still there" >&2; left=1; }
	done
	return $left
}
unlink_mirror || { echo "pgo: dist/pgo is not removed while a junction into the project is left in it" >&2; exit 1; }
trap unlink_mirror EXIT

rm -rf "$OUT"
mkdir -p "$OUT/bin" "$OUT/raw" "$OUT/logs" "$ROOT/native/pgo"
timeout "$CONFIGURE_DEADLINE_S" cmake -S native -B "$BUILD" -G Ninja -DCMAKE_BUILD_TYPE=Release -DGODOTCPP_TARGET=template_release \
	-DCITY_PROFILE=generate -DCITY_OUTPUT_DIR="$(win "$OUT/bin")/" -DCMAKE_TOOLCHAIN_FILE="$ROOT/native/toolchain-mingw.cmake" 2>&1 \
	| tee "$OUT/logs/configure.txt" || { echo "pgo: the configure failed or passed its deadline of $CONFIGURE_DEADLINE_S s" >&2; exit 1; }
timeout "$BUILD_DEADLINE_S" cmake --build "$BUILD" --target citycore -j "$(nproc)" 2>&1 | tee "$OUT/logs/build.txt" \
	|| { echo "pgo: the build failed or passed its deadline of $BUILD_DEADLINE_S s" >&2; exit 1; }

# The project as it is, with the instrumented DLL in place of the two of project/bin: nothing of the project is touched, an open game neither.
mkdir -p "$MIRROR"
for entry in "$ROOT"/project/* "$ROOT"/project/.godot; do
	name="$(basename "$entry")"
	[ "$name" = bin ] && continue
	if [ -d "$entry" ]; then
		cmd //c mklink //J "$(cygpath -w "$MIRROR/$name")" "$(cygpath -w "$entry")" > /dev/null
	else
		cp "$entry" "$MIRROR/$name"
	fi
done
mkdir -p "$MIRROR/bin"
dll="$(basename "$(ls "$OUT"/bin/libcitycore*.dll | head -n 1)")"
cp "$OUT/bin/$dll" "$MIRROR/bin/"
cp "$ROOT/project/bin/citycore.gdextension.uid" "$MIRROR/bin/"
sed "s#^windows\.\(debug\|release\)\.x86_64 = .*#windows.\1.x86_64 = \"res://bin/$dll\"#" "$ROOT/project/bin/citycore.gdextension" > "$MIRROR/bin/citycore.gdextension"

# Each scene a hidden task of tools/tasks (its own deadline, its tree killed by PID), all at once; each writes its counts to a
# folder of its own, the task's output, keyed by the instrumented DLL and the project.
requests="$OUT/logs/requests.jsonl"
for scene in $SCENES; do mkdir -p "$OUT/raw/$(basename "$scene")"; done
"${PY:-python}" - "$requests" "$GODOT" "$(win "$MIRROR")" "$(win "$OUT/raw")" "$(win "$OUT/bin/$dll")" "$DEADLINE_S" $SCENES <<'PY'
import json, sys
out, godot, mirror, raw, dll, deadline, *scenes = sys.argv[1:]
with open(out, "w", encoding="utf-8") as f:
    for scene in scenes:
        name = scene.rsplit("/", 1)[-1]
        f.write(json.dumps({"id": name, "name": f"pgo-{name}", "timeout_s": float(deadline), "hidden": True,
                            "argv": [godot, "--audio-driver", "Dummy", "--path", mirror, "--resolution", "1600x900",
                                     f"res://{scene}.tscn", "--", "--no-flow"],
                            "env": {"LLVM_PROFILE_FILE": f"{raw}/{name}/%p.profraw"},
                            "inputs": ["project/**", "!project/bin/**", dll], "outputs": [f"{raw}/{name}"]}) + "\n")
PY
# serve writes each task's lines and its progress to stderr (the terminal) as they come, and the results to stdout (the file)
"${PY:-python}" tools/tasks/cli.py serve < "$requests" > "$OUT/logs/results.jsonl" || exit 1
"${PY:-python}" -c "import json,sys; r=[json.loads(l) for l in open(sys.argv[1])]; bad=[x for x in r if x['status'] not in ('PASS','CACHED_PASS')]; [print(x['id'], x['status'], x.get('log') or x.get('error')) for x in bad]; sys.exit(1 if bad or len(r) != int(sys.argv[2]) else 0)" "$OUT/logs/results.jsonl" "$(echo $SCENES | wc -w)" || exit 1

ls "$OUT"/raw/*/*.profraw > /dev/null 2>&1 || { echo "no profile written"; exit 1; }
timeout "$MERGE_DEADLINE_S" "$MINGW/bin/llvm-profdata.exe" merge -o "$ROOT/native/pgo/citycore.profdata" "$OUT"/raw/*/*.profraw || exit 1
timeout "$MERGE_DEADLINE_S" "$MINGW/bin/llvm-profdata.exe" show "$ROOT/native/pgo/citycore.profdata" | head -n 6
echo "profile: $(du -h "$ROOT/native/pgo/citycore.profdata" | cut -f1)"
