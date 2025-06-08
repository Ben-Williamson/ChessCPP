cmake_minimum_required(VERSION 3.3)
include(FetchContent)
project(Chess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(tracy)

add_executable(Chess main.cpp
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
)

add_executable(MagicSearch magics/main.cpp
        magics/MagicSearch.cpp
        magics/MagicSearch.h

)

target_link_libraries(Chess PUBLIC TracyClient)

