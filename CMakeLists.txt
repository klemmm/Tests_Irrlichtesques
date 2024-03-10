cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-O3 -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "-pg -O3 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address -fsanitize=undefined")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-g -fsanitize=address")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-pg")

project(TutoIrrlicht LANGUAGES CXX)

set( SOURCES
	main.cpp entity.cpp chunk.cpp world.cpp assets.cpp transforms.cpp building.cpp ship.cpp hud.cpp octree.cpp fpnum.cpp debug.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Irrlicht)
