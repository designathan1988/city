#!/usr/bin/env bash
# convert.sh <game copy> <work dir>: road-elevation (world/elevation.ts of the 4.0) through the converter (tools/converter)
# against a copy of the game: the copy's compile commands exported, its C++ graph (clangd), the 4.0's TypeScript graph, then
# `convert` with this port's library (the converter's own plus the game's names of the road tuning). Events, one a line, in
# <work dir>/events.log; the conversion (files/, ledger.json, conversao.md) in <work dir>/conversion.
set -uo pipefail
GAME="$(cd "$1" && pwd)"; WORK="$2"
HERE="$(cd "$(dirname "$0")" && pwd)"
CONV="${CONVERTER:-$GAME/tools/converter}" # the converter's own repository (its newest master) when set
SRC="${ROADCRAFT_40:-C:/Users/Jonathan/Documents/Projects/roadcraft-4.0-main}"
MINGW="${MINGW:-C:/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64}"
BOUNDARY="$GAME/tools/process/converter-old/out/features/road-elevation/boundary.json"
mkdir -p "$WORK"
EV="$WORK/events.log"
: > "$EV"
ev() { echo "$(date +%T) $*" >> "$EV"; }
w() { cygpath -m "$1"; }

ev "INICIO compile_commands"
cmake -S "$GAME/native" -B "$GAME/native/build-win-debug" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON > "$WORK/cmake.log" 2>&1
ev "FIM compile_commands $?"
cd "$CONV" || exit 2
ev "INICIO cppgraph"
node src/cli.ts cppgraph --root "$(w "$GAME")" --compile-commands "$(w "$GAME/native/build-win-debug/compile_commands.json")" \
	--clangd "$MINGW/bin/clangd.exe" "--clangd-arg=--query-driver=$MINGW/bin/x86_64-w64-mingw32-clang++.exe" --include native/src \
	--cache "$(w "$WORK/clangd")" --out "$(w "$WORK/cppgraph.json")" > "$WORK/cppgraph.log" 2>&1
ev "FIM cppgraph $? $(tail -n 1 "$WORK/cppgraph.log" | cut -c1-200)"
ev "INICIO graph"
node src/cli.ts graph --source-root "$SRC" --out "$(w "$WORK/graph.json")" > "$WORK/graph.log" 2>&1
ev "FIM graph $? $(tail -n 1 "$WORK/graph.log" | cut -c1-200)"
[ -f "$HERE/constants.json" ] || echo '{}' > "$HERE/constants.json"
ev "INICIO convert"
node src/cli.ts convert --feature road-elevation --source-root "$SRC" --typescript "$SRC/node_modules/typescript" \
	--graph "$(w "$WORK/graph.json")" --boundary "$(w "$BOUNDARY")" --target-root "$(w "$GAME")" \
	--target-graph "$(w "$WORK/cppgraph.json")" --library "$(w "$HERE/conversion-library.json")" --ports "$(w "$GAME/tools/ports")" \
	--cxx "$MINGW/bin/x86_64-w64-mingw32-clang++.exe" --constants "$(w "$HERE/constants.json")" --out "$(w "$WORK/conversion")" \
	> "$WORK/convert.log" 2>&1
ev "FIM convert $? $(tail -n 1 "$WORK/convert.log" | cut -c1-300)"
ev "FIM DA FILA"
