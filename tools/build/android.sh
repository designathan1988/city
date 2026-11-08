#!/usr/bin/env bash
# android.sh: the Android build (documented in CLAUDE.md > Comandos). Usage: tools/build/android.sh <core|export>.
# core: the arm64 core library, template_release, API 24 as godot-cpp; the export's debug build loads it too (citycore.gdextension).
# No PGO profile: native/pgo was trained on the PC build. export: the APK (dist/android/Roadcraft.apk), signed with the release
# keystore of GODOT_ANDROID_KEYSTORE_RELEASE_{PATH,USER,PASSWORD}, read from KEYSTORE_ENV when it exists. The tools default to
# C:/Projects/toolchains: NDK r28b, JDK 17, the SDK with build-tools 35.0.1 (Godot reads the last two from its editor settings).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
TOOLCHAINS="${TOOLCHAINS:-C:/Projects/toolchains}"
ANDROID_NDK="${ANDROID_NDK:-$TOOLCHAINS/android-sdk/ndk/28.1.13356709}"
KEYSTORE_ENV="${KEYSTORE_ENV:-$TOOLCHAINS/keystores/roadcraft-release.env}"
GODOT="${GODOT:-godot}"
OUT="${OUT:-$ROOT/dist}"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 2)}"
BUILD="$ROOT/native/build-android"

core() {
	cmake -S "$ROOT/native" -B "$BUILD" -G Ninja -DCMAKE_BUILD_TYPE=Release -DGODOTCPP_TARGET=template_release -DCITY_PROFILE=none \
		-DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24
	cmake --build "$BUILD" --target citycore -j "$JOBS"
}

export_apk() {
	# shellcheck source=/dev/null
	[ -f "$KEYSTORE_ENV" ] && . "$KEYSTORE_ENV"
	: "${GODOT_ANDROID_KEYSTORE_RELEASE_PATH:?no release keystore: set GODOT_ANDROID_KEYSTORE_RELEASE_{PATH,USER,PASSWORD} or KEYSTORE_ENV}"
	export GODOT_ANDROID_KEYSTORE_RELEASE_PATH GODOT_ANDROID_KEYSTORE_RELEASE_USER GODOT_ANDROID_KEYSTORE_RELEASE_PASSWORD
	mkdir -p "$OUT/android"
	"$GODOT" --headless --path "$ROOT/project" --export-release "Android" "$OUT/android/Roadcraft.apk"
}

case "${1:-}" in
	core) core ;;
	export) export_apk ;;
	*) echo "targets: core | export"; exit 2 ;;
esac
