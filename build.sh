#!/bin/bash
cd "${0%/*}"

for arg in "$@"; do eval "$arg=1"; done

if [ "$help" == "1" ]; then
    echo "./build.sh <arg 1> <arg 2> ... <arg n>"
    echo ""
    echo "valid args"
    echo "----------"
    echo "pf: compile platform"
    echo "mk: compile editor"
    echo "clean: delete out/"
    echo "debug: debug build/"
    echo "release: release build/"
fi

common_flags="-std=c++17 -msse4.1 -fno-rtti -fno-exceptions -Wall -Wno-unused-function -Wno-writable-strings -Wno-comment"

debug_build="-O0 -g"
release_build="-O3"

[ "$pf" != "1" ] && [ "$mk" != "1" ] && pf="1" && mk="1" 
[ "$debug" == "1" ] && build_type="$debug_build" 
[ "$release" == "1" ] && build_type="$release_build"
[ "$release" != "1" ] && build_type="$debug_build"

[ "$clean" == "1" ] && echo "deleted /out" && rm -rf "out/"

[ "$build_type" == "$debug_build" ] && echo "[debug build]"
[ "$build_type" == "$release_build" ] && echo "[release build]"

[ ! -d "out" ] && mkdir "out"

[ "$pf" == "1" ] && clang++ $common_flags $build_type code/main.cpp -o "out/yk" && echo "compiled platform"

[ "$mk" == "1" ] && clang++ $common_flags $build_type -fPIC -shared code/mk_editor.cpp -o "out/libyk.so" && echo "compiled editor"