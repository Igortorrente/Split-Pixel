cmake_minimum_required(VERSION 3.6.2)
project(Split_Pixel_Serial)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP)
FIND_LIBRARY(FREE_IMAGE_LIBRARY
        NAMES ${LIB_PREFIX}freeimage.${LIB_POSTFIX}
        HINTS ${FREE_IMAGE_LIBRARY_DIR}
        NO_DEFAULT_PATH
        )

#set(CMAKE_C_FLAGS "-Wall -pedantic  -Wno-multichar ${OpenMP_C_FLAGS} -lm -g")
set(CMAKE_C_FLAGS "-Wall -pedantic -Wno-multichar ${OpenMP_C_FLAGS} -D DEBUG1 -lm -o2 -mtune=native")

add_executable(Split_Pixel_Serial main.c randombytes/randombytes.c)

target_link_libraries(${PROJECT_NAME} freeimage)
