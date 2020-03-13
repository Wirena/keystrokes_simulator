cmake_minimum_required(VERSION 3.15)
project(CtrlV_KeyPaster)

set(CMAKE_CXX_STANDARD 14)

add_executable(CtrlV_KeyPaster gui.h gui.cpp input_emulator.h input_emulator.cpp ptr.h main.cpp header.h resource.rc)

target_link_libraries(CtrlV_KeyPaster comctl32 UxTheme)
