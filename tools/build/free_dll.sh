#!/usr/bin/env bash
# free_dll.sh: a game left open holds its DLL, and the linker cannot write over it. Windows lets a file in use be renamed: each DLL goes aside under
# dist/held, the open game keeps running from it, and a copy stands in its place for the link to replace. Old ones go when nothing holds them.
# Usage: free_dll.sh [name.dll ...]: only the DLLs the build will write (every one when none is named), so the build of one
# never moves the other while other work reads it (the tests key their inputs on the debug DLL while the release one links).
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
mkdir -p "$ROOT/dist/held"
rm -f "$ROOT"/dist/held/*.dll 2>/dev/null
for dll in "$ROOT"/project/bin/*.dll; do
	[ -f "$dll" ] || continue
	if [ $# -gt 0 ]; then case " $* " in *" $(basename "$dll") "*) ;; *) continue ;; esac; fi
	held="$ROOT/dist/held/$(basename "$dll" .dll).$$.dll"
	mv "$dll" "$held" && cp "$held" "$dll"
done
exit 0
