#!/bin/bash
# OOP Final Project 실행 스크립트

# 컴파일된 실행 파일이 없으면 먼저 컴파일
if [ ! -f "./game" ]; then
    echo "No executable found. Compiling..."
    ./compile.sh
    if [ $? -ne 0 ]; then
        exit 1
    fi
fi

echo "game working..."
./game
