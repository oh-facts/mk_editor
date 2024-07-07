#!/bin/bash
cd "${0%/*}"

for arg in "$@"; do eval "$arg=1"; done

if [ "$help" == "1" ]; then
    echo "./build.sh <arg 1> <arg 2> ... <arg n>"
    echo ""
    echo "valid args"
    echo "----------"
    echo "mk: compile editor"
    echo "pf: compile platform"
    echo "clean: delete out/"
fi

common_flags="-std=c++17 -msse4.1 -O0 -fno-rtti -fno-exceptions -Wall -Wno-unused-function -Wno-writable-strings -Wno-comment -g"

[ "$clean" == "1" ] && echo "deleted /out" && rm -rf "out/"

[ ! -d "out" ] && mkdir "out"

[ "$pf" == "1" ] && echo "compiled platform" && clang++ $common_flags code/main.cpp -o "out/yk"

[ "$mk" == "1" ] && echo "compiled editor" && clang++ $common_flags -fPIC -shared code/mk_editor.cpp -o "out/libyk.so"