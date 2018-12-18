cmake_minimum_required(VERSION 3.6.2)
project(Split_Pixel_Serial)

set(CMAKE_C_STANDARD 11)

FIND_LIBRARY(FREE_IMAGE_LIBRARY
        NAMES ${LIB_PREFIX}freeimage.${LIB_POSTFIX}
        HINTS ${FREE_IMAGE_LIBRARY_DIR}
        NO_DEFAULT_PATH
        )

#set(CMAKE_C_FLAGS "-Wall -pedantic -lm -g -Wno-multichar")
set(CMAKE_C_FLAGS "-Wall -pedantic -Wno-multichar -D DEBUG1 -lm -o3 -mtune=native")

add_executable(Split_Pixel_Serial main.c randombytes/randombytes.c)

target_link_libraries(${PROJECT_NAME} freeimage)
