#!/usr/bin/env bash
set -euo pipefail

DEFAULT_PREFIX="${HOME}/.local"
BUILD_DIR="build"          
CMAKE_BUILD_TYPE="Release"
USER_BIN="${HOME}/.local/bin"

cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"

cmake --build "${BUILD_DIR}" --config "${CMAKE_BUILD_TYPE}"

cmake --install "${BUILD_DIR}" --prefix "${DEFAULT_PREFIX}"

echo
echo "Install finished."

if ! echo "$PATH" | tr ':' '\n' | grep -qx "$USER_BIN"; then
  echo
  echo "Note: $USER_BIN is not in your PATH. Check The ReadMe For Install Instructions"
else
  echo "$USER_BIN is already on PATH — good to go!"
fi