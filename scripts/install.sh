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
  echo ""
    echo "!!! '$USER_BIN' is not in your PATH."
    echo "This means you cannot run 'ascii-generator' globally yet."
    echo ""
    echo "Options:"
    echo "  • Press ENTER to let this script add it automatically."
    echo "  • Type 'n' then ENTER if you want to do it manually."
    echo ""
    read -r -p "Add $USER_BIN to PATH automatically? [Y/n]: " answer

    if [ "$answer" = "n" ] || [ "$answer" = "N" ]; then
        echo ""
        echo "Okay. Add this line to your shell profile manually:"
        echo ""
        echo "For more information check out the ReadMe on Github!!"
        echo ""
        echo "    export PATH=\"\$HOME/.local/bin:\$PATH\""
        echo ""
        echo "After adding, restart your terminal."
        exit 0
    fi

    echo ""
    echo "Adding $USER_BIN to your PATH..."

    add_line() {
        local file="$1"
        local line="$2"
        mkdir -p "$(dirname "$file")" 2>/dev/null || true
        if [ -f "$file" ]; then
            grep -qxF "$line" "$file" || echo "$line" >> "$file"
        else
            echo "$line" >> "$file"
        fi
    }

    EXPORT_LINE='export PATH="$HOME/.local/bin:$PATH"'

    # Always add to ~/.profile (login shell)
    add_line "$HOME/.profile" "$EXPORT_LINE"

    # Detect shell
    CURRENT_SHELL=$(basename "$SHELL")

    case "$CURRENT_SHELL" in
        bash)
            add_line "$HOME/.bashrc" "$EXPORT_LINE"
            SHELL_RC="$HOME/.bashrc"
            ;;
        zsh)
            add_line "$HOME/.zshrc" "$EXPORT_LINE"
            SHELL_RC="$HOME/.zshrc"
            ;;
        fish)
            # fish has a unique way of handling PATH
            if command -v fish >/dev/null 2>&1; then
                fish -c "contains $USER_BIN \$fish_user_paths || set -Ux fish_user_paths $USER_BIN \$fish_user_paths"
            fi
            SHELL_RC="fish_user_paths"
            ;;
        *)
            # Fallback: only ~/.profile
            SHELL_RC="$HOME/.profile"
            ;;
    esac

    # Apply PATH to this session
    export PATH="$HOME/.local/bin:$PATH"
    hash -r 2>/dev/null || true

    echo ""
    echo "✅ PATH updated!"
    echo ""
    echo "Added to:"
    echo "  - ~/.profile"
    echo "  - $SHELL_RC"
    echo ""
    echo "You can now run:"
    echo "    ascii-generator --help but first -> "
    echo ""
    echo "Restart your terminal"
else
  echo "$USER_BIN is already on PATH — good to go!"
fi