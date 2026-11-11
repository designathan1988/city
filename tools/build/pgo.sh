#!/usr/bin/env bash
# pgo.sh: the profile the release DLL is optimized by (native/pgo/citycore.profdata). An instrumented DLL, built as the export's, plays
# the game hidden: the camera over a running city, edits, roads, people, yards. The counts are merged and committed; a build then
# needs no training. Stale functions only lose their counts, so a new profile is a choice, not a duty: after a change of the hot paths.
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT" || exit 2
# The machine's tools from their one owner (tools/tasks/locations.py: the environment, tools/tasks/local.json, the PATH)
MINGW="$("${PY:-python}" tools/tasks/locations.py mingw)" || exit 2
GODOT="$("${PY:-python}" tools/tasks/locations.py godot)" || exit 2
export MINGW
OUT="$ROOT/dist/pgo"
BUILD="$ROOT/dist/build-pgo"
SCENES="tests/perf/camera_pan tests/core/edit_fuzz_test tests/core/road_commands_test tests/core/people_test tests/core/yard_test"
DEADLINE_S=900
win() { cygpath -m "$1"; }

rm -rf "$OUT"
mkdir -p "$OUT/bin" "$OUT/raw" "$OUT/logs" "$ROOT/native/pgo"
cmake -S native -B "$BUILD" -G Ninja -DCMAKE_BUILD_TYPE=Release -DGODOTCPP_TARGET=template_release -DCITY_PROFILE=generate \
	-DCITY_OUTPUT_DIR="$(win "$OUT/bin")/" -DCMAKE_TOOLCHAIN_FILE="$ROOT/native/toolchain-mingw.cmake" > "$OUT/logs/configure.txt" 2>&1 || exit 1
cmake --build "$BUILD" --target citycore -j "$(nproc)" > "$OUT/logs/build.txt" 2>&1 || { tail -n 20 "$OUT/logs/build.txt"; exit 1; }

# The project as it is, with the instrumented DLL in place of the two of project/bin: nothing of the project is touched, an open game neither.
mirror="$OUT/project"
mkdir -p "$mirror"
for entry in "$ROOT"/project/* "$ROOT"/project/.godot; do
	name="$(basename "$entry")"
	[ "$name" = bin ] && continue
	if [ -d "$entry" ]; then
		cmd //c mklink //J "$(cygpath -w "$mirror/$name")" "$(cygpath -w "$entry")" > /dev/null
	else
		cp "$entry" "$mirror/$name"
	fi
done
mkdir -p "$mirror/bin"
dll="$(basename "$(ls "$OUT"/bin/libcitycore*.dll | head -n 1)")"
cp "$OUT/bin/$dll" "$mirror/bin/"
cp "$ROOT/project/bin/citycore.gdextension.uid" "$mirror/bin/"
sed "s#^windows\.\(debug\|release\)\.x86_64 = .*#windows.\1.x86_64 = \"res://bin/$dll\"#" "$ROOT/project/bin/citycore.gdextension" > "$mirror/bin/citycore.gdextension"

# Each scene a hidden task of tools/tasks (its own deadline, its tree killed by PID), all at once; each writes its counts to a
# folder of its own, the task's output, keyed by the instrumented DLL and the project.
requests="$OUT/logs/requests.jsonl"
for scene in $SCENES; do mkdir -p "$OUT/raw/$(basename "$scene")"; done
"${PY:-python}" - "$requests" "$GODOT" "$(win "$mirror")" "$(win "$OUT/raw")" "$(win "$OUT/bin/$dll")" "$DEADLINE_S" $SCENES <<'PY'
import json, sys
out, godot, mirror, raw, dll, deadline, *scenes = sys.argv[1:]
with open(out, "w", encoding="utf-8") as f:
    for scene in scenes:
        name = scene.rsplit("/", 1)[-1]
        f.write(json.dumps({"id": name, "name": f"pgo-{name}", "timeout_s": float(deadline), "hidden": True,
                            "argv": [godot, "--path", mirror, "--resolution", "1600x900", f"res://{scene}.tscn", "--", "--no-flow"],
                            "env": {"LLVM_PROFILE_FILE": f"{raw}/{name}/%p.profraw"},
                            "inputs": ["project/**", "!project/bin/**", dll], "outputs": [f"{raw}/{name}"]}) + "\n")
PY
"${PY:-python}" tools/tasks/cli.py serve < "$requests" > "$OUT/logs/results.jsonl" || exit 1
"${PY:-python}" -c "import json,sys; r=[json.loads(l) for l in open(sys.argv[1])]; bad=[x for x in r if x['status'] not in ('PASS','CACHED_PASS')]; [print(x['id'], x['status'], x.get('log') or x.get('error')) for x in bad]; sys.exit(1 if bad or len(r) != int(sys.argv[2]) else 0)" "$OUT/logs/results.jsonl" "$(echo $SCENES | wc -w)" || exit 1

ls "$OUT"/raw/*/*.profraw > /dev/null 2>&1 || { echo "no profile written"; exit 1; }
"$MINGW/bin/llvm-profdata.exe" merge -o "$ROOT/native/pgo/citycore.profdata" "$OUT"/raw/*/*.profraw || exit 1
"$MINGW/bin/llvm-profdata.exe" show "$ROOT/native/pgo/citycore.profdata" | head -n 6
# the mirror's junctions lead into the project: taken away first, so nothing deletes through them
for entry in "$mirror"/*; do [ -d "$entry" ] && cmd //c rmdir "$(cygpath -w "$entry")" > /dev/null 2>&1; done
cmd //c rmdir "$(cygpath -w "$mirror/.godot")" > /dev/null 2>&1
echo "profile: $(du -h "$ROOT/native/pgo/citycore.profdata" | cut -f1)"
