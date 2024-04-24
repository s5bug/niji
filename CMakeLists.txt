cmake_minimum_required(VERSION 3.28)
project(niji)

set(CMAKE_CXX_STANDARD 23)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

if (MSVC)
    add_compile_options(/WX /W4 /DUNICODE /D_UNICODE /DWIN32_LEAN_AND_MEAN /MD)
else()
    add_compile_options(-Werror -Wall -Wextra -Wpedantic -DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN)
endif()

add_executable(niji main.cpp)
if (MSVC)
    target_link_libraries(niji PRIVATE Dwmapi)
    add_link_options(/MD)
else()
    target_link_libraries(niji PRIVATE -static-libgcc -Wl,-Bstatic -lstdc++ -lwinpthread -Wl,-Bdynamic -lDwmapi)
endif()

if (ipo_supported AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    set_property(TARGET niji PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if (ipo_supported AND (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo"))
    set_property(TARGET niji PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if (ipo_supported AND (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
    set_property(TARGET niji PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
