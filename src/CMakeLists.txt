cmake_minimum_required(VERSION 3.3)
include(FetchContent)
project(Chess)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -fopenmp")

FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(tracy)

add_executable(CLI_Chess main.cpp
        Board.cpp
        Board.h
        Search.cpp
        Search.h
        TreeDebug.cpp
        TreeDebug.h
        Profiler.h
        MoveList.cpp
        MoveList.h
        Move.h
        magics/MagicSearch.cpp
        magics/MagicSearch.h
        PerformantBoard.cpp
        PerformantBoard.h
        CrossPlatform.h
 "TranspositionTable.cpp")

add_executable(Chess UCI.cpp
        Board.cpp
        Board.h
        Search.cpp
        Search.h
        TreeDebug.cpp
        TreeDebug.h
        Profiler.h
        MoveList.cpp
        MoveList.h
        Move.h
        magics/MagicSearch.cpp
        magics/MagicSearch.h
        PerformantBoard.cpp
        PerformantBoard.h
        CrossPlatform.h
        UCI.h
        Zobrist.h
        Zobrist.cpp
        Random.h
 "TranspositionTable.cpp")

add_executable(MagicSearch magics/main.cpp
        magics/MagicSearch.cpp
        magics/MagicSearch.h
        CrossPlatform.h
 "TranspositionTable.cpp")

add_executable(BoardTest board_test/main.cpp
        Board.cpp
        Board.h
        PerformantBoard.cpp
        PerformantBoard.h
        magics/MagicSearch.cpp
        magics/MagicSearch.h
        CrossPlatform.h
 "TranspositionTable.cpp")

target_link_libraries(Chess PUBLIC TracyClient)

