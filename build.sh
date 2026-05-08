#!/bin/bash

TARGET=${1:-MinecraftClient}
TYPE=${2:-release}

if [ "$TYPE" = "release" ]; then
    cmake --preset release
    cmake --build --preset release -t "$TARGET"
elif [ "$TYPE" = "debug" ]; then
    cmake --preset debug
    cmake --build --preset debug -t "$TARGET"
else
    echo "Usage: ./build.sh [target] [debug|release]"
    echo "  target: MinecraftClient (default) or MinecraftServer"
    echo "  type:   debug (default) or release"
    exit 1
fi
