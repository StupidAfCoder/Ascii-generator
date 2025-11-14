#!/usr/bin/env bash
set -euo pipefail

DEFAULT_PREFIX="${HOME}/.local"
BUILD_DIR="build"          
CMAKE_BUILD_TYPE="Release"

cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"

cmake --build "${BUILD_DIR}" --config "${CMAKE_BUILD_TYPE}"

cmake --install "${BUILD_DIR}" --prefix "${DEFAULT_PREFIX}"

echo
echo "Install finished. Make sure '${DEFAULT_PREFIX}/bin' is on your PATH."