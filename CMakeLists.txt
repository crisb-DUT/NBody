cmake_minimum_required(VERSION 2.6)
project(NBody_Linux)

# Use our modified FindSDL2* modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${NBody_Linux_SOURCE_DIR}/cmake")
set(BIN_DIR ${TwinklebearDevLessons_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIR})
include_directories(src)



add_executable(NBody_Linux
        src/LTexture.cpp
        src/LTexture.h
        src/main.cpp
        src/main.h
        src/NBody.h
        src/NBody.cpp)

target_link_libraries(NBody_Linux ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY})

install(FILES ${PROJECT_SOURCE_DIR}/resource/lazy.ttf DESTINATION ${PROJECT_SOURCE_DIR}/Debug/bin)
install(FILES ${PROJECT_SOURCE_DIR}/resource/lazy.ttf DESTINATION ${PROJECT_SOURCE_DIR}/Release/bin)