#!/bin/bash
set -e

bear --output .vscode/compile_commands.json -- ./build.sh
sed -i 's|'$PWD'|${workspaceFolder}|g' .vscode/compile_commands.json
