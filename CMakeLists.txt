cmake_minimum_required(VERSION 4.0)
project(rbxsdkex)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
add_executable(rbxsdkex main.cpp
        include/rbx/offsets.h
        include/rbx/core.h
        include/rbx/instance.h
        include/rbx/datamodel.h
        include/rbx/player.h
        "include/rbx/camera + visualengine.h"
        include/rbx/vector.h
        include/rbx/sdk.h
)
