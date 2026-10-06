#!/usr/bin/env bash
# Builds the browser (WebAssembly) version of the game into web/dist/.
# The first run downloads Emscripten, Allegro 5 and minimp3 into web/.cache
# (~1 GB) and builds Allegro for the browser; later runs reuse them.
# Emscripten needs Python 3.10+ (set EMSDK_PYTHON if `python3` is older).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
WEB="$ROOT/web"
CACHE="$WEB/.cache"
DIST="$WEB/dist"
ALLEGRO_VERSION=5.2.10.1
PORTS=(-sUSE_SDL=2 -sUSE_FREETYPE=1 -sUSE_VORBIS=1 -sUSE_OGG=1 -sUSE_LIBJPEG=1 -sUSE_LIBPNG=1)
mkdir -p "$CACHE"

# 1. Emscripten
if ! command -v emcc >/dev/null; then
  [ -d "$CACHE/emsdk" ] || git clone --depth 1 https://github.com/emscripten-core/emsdk.git "$CACHE/emsdk"
  if [ ! -x "$CACHE/emsdk/upstream/emscripten/emcc" ]; then
    "$CACHE/emsdk/emsdk" install latest
    "$CACHE/emsdk/emsdk" activate latest
  fi
  source "$CACHE/emsdk/emsdk_env.sh" >/dev/null 2>&1
fi
EM_CACHE="$(em-config CACHE)"

# 2. Allegro 5 for the browser (SDL2 backend, as in Allegro's README_sdl.txt)
AL="$CACHE/allegro-$ALLEGRO_VERSION"
if [ ! -f "$AL/install/lib/liballegro_monolith-static.a" ]; then
  [ -d "$CACHE/minimp3" ] || git clone --depth 1 https://github.com/lieff/minimp3.git "$CACHE/minimp3"
  [ -d "$AL/src" ] || git clone --depth 1 --branch "$ALLEGRO_VERSION" https://github.com/liballeg/allegro5.git "$AL/src"
  # Build the Emscripten ports once so their headers exist for CMake to find.
  echo 'int main(void){return 0;}' > "$CACHE/empty.c"
  emcc "${PORTS[@]}" "$CACHE/empty.c" -o "$CACHE/empty.js"
  FLAGS="${PORTS[*]} -O2 -I\"$CACHE/minimp3\""
  emcmake cmake -S "$AL/src" -B "$AL/build" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$AL/install" \
    -DALLEGRO_SDL=ON -DSHARED=OFF -DWANT_MONOLITH=ON -DWANT_ALLOW_SSE=OFF \
    -DWANT_DOCS=OFF -DWANT_TESTS=OFF -DWANT_EXAMPLES=OFF -DWANT_DEMO=OFF \
    -DWANT_OPENAL=OFF -DWANT_NATIVE_DIALOG=OFF -DWANT_VIDEO=OFF -DWANT_PHYSFS=OFF \
    -DALLEGRO_WAIT_EVENT_SLEEP=ON \
    -DSDL2_INCLUDE_DIR="$EM_CACHE/sysroot/include" \
    -DMINIMP3_INCLUDE_DIRS="$CACHE/minimp3" \
    -DVORBISFILE_LIBRARY="$EM_CACHE/sysroot/lib/wasm32-emscripten/libvorbis.a" \
    -DCMAKE_C_FLAGS="$FLAGS" -DCMAKE_CXX_FLAGS="$FLAGS" -DCMAKE_EXE_LINKER_FLAGS="${PORTS[*]}"
  cmake --build "$AL/build" -j 8
  cmake --install "$AL/build"
fi

# FULL_ES2 emulates the client-side vertex arrays Allegro uses; ASYNCIFY lets
# the game's blocking al_wait_for_event() loop run in the browser.
LINK=(-I"$AL/install/include" "$AL/install/lib/liballegro_monolith-static.a" "${PORTS[@]}"
      -sFULL_ES2=1 -sASYNCIFY -sALLOW_MEMORY_GROWTH=1 -sINITIAL_MEMORY=64MB
      -sINVOKE_RUN=0 -sEXPORTED_RUNTIME_METHODS=callMain --shell-file "$WEB/shell.html")
STAGE="$CACHE/stage"
rm -rf "$STAGE" "$DIST"
mkdir -p "$STAGE" "$DIST"
cd "$ROOT"

# 3. The game itself
cp ./*.png ./*.mp3 ./*.wav leaderboard.txt "$STAGE/"
em++ -std=c++17 -O2 ./*.cpp "${LINK[@]}" --preload-file "$STAGE@/" -o "$DIST/index.html"
echo "Built $DIST - serve it with: python3 -m http.server -d web/dist"
