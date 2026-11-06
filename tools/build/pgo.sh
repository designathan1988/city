#!/usr/bin/env bash
# pgo.sh: the profile the release DLL is optimized by (native/pgo/citycore.profdata). An instrumented DLL, built as the export's, plays
# the game hidden: the camera over a running city, edits, roads, people, yards. The counts are merged and committed; a build then
# needs no training. Stale functions only lose their counts, so a new profile is a choice, not a duty: after a change of the hot paths.
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT" || exit 2
export MINGW="${MINGW:-C:/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64}"
GODOT="C:/Users/Jonathan/AppData/Local/Microsoft/WinGet/Links/godot_console.exe"
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

export LLVM_PROFILE_FILE="$(win "$OUT/raw")/%p.profraw"
pids=""
for scene in $SCENES; do
	name="$(basename "$scene")"
	powershell -NoProfile -ExecutionPolicy Bypass -File "$(win "$ROOT/tools/build/hidden_run.ps1")" -TimeoutS "$DEADLINE_S" -WorkDir "$(win "$ROOT")" \
		-Command "\"$GODOT\" --path \"$(win "$mirror")\" --resolution 1600x900 res://$scene.tscn -- --no-flow > \"$(win "$OUT/logs/$name.txt")\" 2>&1" > "$OUT/logs/$name.run" 2>&1 &
	pids="$pids $!"
done
for pid in $pids; do wait "$pid"; done

ls "$OUT"/raw/*.profraw > /dev/null 2>&1 || { echo "no profile written"; exit 1; }
"$MINGW/bin/llvm-profdata.exe" merge -o "$ROOT/native/pgo/citycore.profdata" "$OUT"/raw/*.profraw || exit 1
"$MINGW/bin/llvm-profdata.exe" show "$ROOT/native/pgo/citycore.profdata" | head -n 6
# the mirror's junctions lead into the project: taken away first, so nothing deletes through them
for entry in "$mirror"/*; do [ -d "$entry" ] && cmd //c rmdir "$(cygpath -w "$entry")" > /dev/null 2>&1; done
cmd //c rmdir "$(cygpath -w "$mirror/.godot")" > /dev/null 2>&1
echo "profile: $(du -h "$ROOT/native/pgo/citycore.profdata" | cut -f1)"
