#cmake_minimum_required(VERSION 3.6)

#project(MY_APP)

#find_library(SDL2 SDL2)

#add_library(main SHARED)

#target_sources(main PRIVATE YourSourceHere.c)

#target_link_libraries(main SDL2)

cmake_minimum_required(VERSION 3.6)
project(MY_APP)

file(GLOB GAME_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(main SHARED ${GAME_SOURCES})

target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(main
    SDL2
    SDL2_image
    SDL2_mixer
    z
)
