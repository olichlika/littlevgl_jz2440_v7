cmake_minimum_required(VERSION 3.10)
project(Ui C)

SET(CROSS_COMPILE 1)
set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_C_COMPILER "/root/jz2440_v3/gcc-3.4.5-glibc-2.3.6/bin/arm-linux-gcc")
set(CMAKE_CXX_COMPILER "/root/jz2440_v3/gcc-3.4.5-glibc-2.3.6/bin/arm-linux-gcc++")

SET(TOOLCHAIN_DIR "/root/jz2440_v3/gcc-3.4.5-glibc-2.3.6/bin/")

SET(CMAKE_FIND_ROOT_PATH  "/root/jz2440_v3/gcc-3.4.5-glibc-2.3.6" "/root/jz2440_v3/gcc-3.4.5-glibc-2.3.6/lib" "/root/jz2440_v3/gcc-3.4.5-glibc-2.3.6/include")

set(CMAKE_C_FLAGS "-lpthread")

include_directories(/root/jz2440_v3/busybox_install_3.4.5/include)

link_directories(/root/jz2440_v3/busybox_install_3.4.5/lib)

file(GLOB_RECURSE INCLUDES "lv_drivers/*.h" "lv_examples/*.h"  "lvgl/*.h"  "./*.h" "tslib/*.h")
file(GLOB_RECURSE SOURCES  "lv_drivers/*.c" "lv_examples/*.c"  "lvgl/*.c" "tslib/*.c")

include_directories(.)
include_directories(tslib)
include_directories(lv_drivers/display)
include_directories(lv_drivers/indev)

add_executable(Ui ${SOURCES} ${INCLUDES} main.c)

TARGET_LINK_LIBRARIES(Ui dl)