#!/usr/bin/env bash
# free_dll.sh: a game left open holds its DLL, and the linker cannot write over it. Windows lets a file in use be renamed: each DLL goes aside under
# dist/held, the open game keeps running from it, and a copy stands in its place for the link to replace. Old ones go when nothing holds them.
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
mkdir -p "$ROOT/dist/held"
rm -f "$ROOT"/dist/held/*.dll 2>/dev/null
for dll in "$ROOT"/project/bin/*.dll; do
	[ -f "$dll" ] || continue
	held="$ROOT/dist/held/$(basename "$dll" .dll).$$.dll"
	mv "$dll" "$held" && cp "$held" "$dll"
done
exit 0
