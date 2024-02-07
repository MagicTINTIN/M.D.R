#!/usr/bin/bash
projectName="bfpp"

mkdir build 2> /dev/null
mkdir debug 2> /dev/null

makearg=""
cmakearg=""
folder="build"
for arg in "$@"; do
    if [ "$arg" = "--release" ]; then
        cmakearg+=" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=\"-O3\" "
    elif [ "$arg" = "--debug" ]; then
        cmakearg+=" -DCMAKE_BUILD_TYPE=Debug "
        makearg+=" VERBOSE=1 "
        folder="debug"
    elif [ "$arg" = "--fastcomile" ]; then
        makearg+=" -j4 "
    elif [ "$arg" = "--reset" ]; then
        rm -rf build/*
        rm -rf debug/*
    fi
done

cd $folder/
cmake $cmakearg ../
if [[ $? != 0 ]]; then
    err=$?
    echo -ne "$(tput setaf 9)"
    echo "Compilation of $projectName went wrong.$(tput sgr0)"
    cd ../
    exit $err
fi
make $makearg
if [[ $? != 0 ]]; then
    err=$?
    echo -ne "$(tput setaf 9)"
    echo "Compilation of $projectName went wrong.$(tput sgr0)"
    cd ../
    exit $err
else
    echo -e "$(tput setaf 2)$(tput bold)Compilation of $projectName finished.$(tput sgr0)\nExecute the compiled program using ./exec.sh [args...]"
fi

cd ../
echo -ne "$(tput sgr0)"
