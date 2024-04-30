cmake_minimum_required(VERSION 3.28.2)

include(../sdk/config.cmake)
project(sieelf_tabfm C)

include_directories("../libsie/src/include")
link_libraries(crt crt_helper c gcc ft sie)
file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(TabFM_ELKA ${SOURCES})
target_sdk_setup(TabFM_ELKA ELKA)
target_link_directories(TabFM_ELKA PUBLIC "../libsie/build/lib/ELKA")
target_compile_options(TabFM_ELKA BEFORE PUBLIC -std=gnu99 -Wall -Werror)
