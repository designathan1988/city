#!/usr/bin/env bash
# Builds the native library and copies it next to the UI.
set -euo pipefail

compile_native() {
  local mode="$1"
  if [ "$mode" = "release" ]; then
    cmake --build build --config Release
  else
    cmake --build build --config Debug
  fi
}

for target in native ui; do
  case "$target" in
    native) compile_native "${1:-debug}" ;;
    ui) echo "ui needs no build" ;;
  esac
done
