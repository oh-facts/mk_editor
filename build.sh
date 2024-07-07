#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"

for arg in "$@"
do
    case $arg in
        app)
            app=1
            ;;        
        app2)
            app2=1
            ;;       
        clean)
            clean=1
            ;;
        *)
            echo "Unknown option: $arg"
            exit 1
            ;;
    esac
done

common_flags="-std=c++17 -msse4.1 -O0 -fno-rtti -fno-exceptions -Wall -Wno-unused-function -Wno-writable-strings -Wno-comment -g"

if [ "$clean" == "1" ]; then
    rm -rf "$SCRIPT_DIR/out"
fi

if [ "$app" == "1" ]; then
    if [ ! -d "$SCRIPT_DIR/out" ]; then
        mkdir "$SCRIPT_DIR/out"
    fi

    clang++ $common_flags code/main.cpp -o "$SCRIPT_DIR/out/yk"
fi

if [ "$app2" == "1" ]; then
    if [ ! -d "$SCRIPT_DIR/out" ]; then
        mkdir "$SCRIPT_DIR/out"
    fi

    clang++ $common_flags -fPIC -shared code/mk_editor.cpp -o "$SCRIPT_DIR/out/libyk.so"
fi

if [ $? -ne 0 ]; then
    echo "Platform compilation failed"
    exit 1
fi