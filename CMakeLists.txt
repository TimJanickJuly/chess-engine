cmake_minimum_required(VERSION 3.28)
project(chess)

set(CMAKE_CXX_STANDARD 17)

add_executable(chess main.cpp
        game.cpp
        game.h
        tmp.cpp
        tmp.h
        move.h
        Move.cpp
        Chess_Piece.cpp
        Chess_Piece.h
        test.h
        test.cpp

)
