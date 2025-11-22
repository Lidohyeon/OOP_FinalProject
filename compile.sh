#!/bin/bash
# OOP Final Project 컴파일 스크립트
# window 환경에서는 g++ 대신에 다른 컴파일러를 사용해야 할 수 있습니다.

echo "project compiling..."

g++ -o game main.cpp SentenceManager.cpp -lncurses -std=c++11

if [ $? -eq 0 ]; then
    echo "✅ compile success!"
    echo "To run: ./run.sh or ./game"
else
    echo "❌ compile failed"
    exit 1
fi
