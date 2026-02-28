cmake_minimum_required(VERSION 3.20)

project(sudoku VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED 11)

# Prefix for option names
set(PREFIX SUDOKU)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(${PREFIX}_ENABLE_WARNING_CFLAGS "Enable warning flags" ON)
option(${PREFIX}_TURN_WARNINGS_INTO_ERRORS "Turn all warnings into errors" ON)
option(${PREFIX}_ENABLE_ADDRESS_SANITIZER "Enable address sanitizer" OFF)
option(${PREFIX}_ENABLE_MEMORY_LEEKS_CHECK "Enable memory leaks check" OFF)
option(ENABLE_SDL_VENDORED "Use vendored sdl (vendored/sdl)" OFF)
option(${PREFIX}_BUILD_TESTS "Build tests" ON)

# Suppress msvc crt secure warnings
if (MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# Set warning flags
if (${PREFIX}_ENABLE_WARNING_CFLAGS)
    set(${PREFIX}_WARNING_CFLAGS "")

    if ((CMAKE_C_COMPILER_ID STREQUAL "Clang") OR (CMAKE_C_COMPILER_ID STREQUAL "GNU"))
        set(${PREFIX}_WARNING_CFLAGS -Wall -Wextra)
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        set(${PREFIX}_WARNING_CFLAGS /W4)
    endif()

    if (${PREFIX}_WARNING_CFLAGS)
        add_compile_options(${${PREFIX}_WARNING_CFLAGS})
    endif()
endif()

if (${PREFIX}_TURN_WARNINGS_INTO_ERRORS)
    set(${PREFIX}_WARNING_CFLAGS "")

    if ((CMAKE_C_COMPILER_ID STREQUAL "Clang") OR (CMAKE_C_COMPILER_ID STREQUAL "GNU"))
        set(${PREFIX}_WARNING_CFLAGS -Werror)
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        set(${PREFIX}_WARNING_CFLAGS /WX)
    endif()

    if (${PREFIX}_WARNING_CFLAGS)
        add_compile_options(${${PREFIX}_WARNING_CFLAGS})
    endif()
endif()

# Debugging and diagnostics
if (${PREFIX}_ENABLE_ADDRESS_SANITIZER)
    set(${PREFIX}_COMPILE_SAN_FLAGS "")
    set(${PREFIX}_LINKER_SAN_FLAGS "")
    if ((CMAKE_C_COMPILER_ID STREQUAL "Clang") OR (CMAKE_C_COMPILER_ID STREQUAL "GNU"))
        set(${PREFIX}_COMPILE_SAN_FLAGS -fsanitize=address)
        set(${PREFIX}_LINKER_SAN_FLAGS -fsanitize=address)
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        set(${PREFIX}_COMPILE_SAN_FLAGS /fsanitize=address)
    endif()

    if (${PREFIX}_COMPILE_SAN_FLAGS)
        add_compile_options(${${PREFIX}_COMPILE_SAN_FLAGS})
    endif()
    
    if (${PREFIX}_LINKER_SAN_FLAGS)
        add_link_options(${${PREFIX}_LINKER_SAN_FLAGS})
    endif()
endif()

if (${PREFIX}_ENABLE_MEMORY_LEEKS_CHECK)
    if ((CMAKE_C_COMPILER_ID STREQUAL "Clang") OR (CMAKE_C_COMPILER_ID STREQUAL "GNU"))
        if (NOT ${PREFIX}_ENABLE_ADDRESS_SANITIZER)
            add_compile_options(-fsanitize=address)
            add_link_options(-fsanitize=address)
        endif()
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        add_compile_definitions(_CRTDBG_MAP_ALLOC)
    endif()
endif()

# Add targets
if(ENABLE_SDL_VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
else()
    find_package(SDL2 CONFIG REQUIRED)
endif()

find_package(SDL2_ttf CONFIG REQUIRED)

add_library(sudoku STATIC 
                    src/lib/generator.c 
                    src/lib/solver.c
                    src/lib/board.c
                    src/lib/sudoku.c
                    src/lib/input.c
                    include/sudoku.h
                    include/solver.h
                    include/board.h
                    include/input.h
                )
target_include_directories(sudoku PUBLIC include)
target_link_libraries(sudoku 
                        PRIVATE 
                        SDL2::SDL2
                        $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Darwin>>:m>
                        draw)

add_library(draw STATIC
                    src/lib/draw.c
                    include/draw.h)
target_link_libraries(draw PRIVATE 
                        SDL2::SDL2
                        SDL2_ttf::SDL2_ttf)
target_include_directories(draw PUBLIC include)

add_executable(app src/app/app.c)
target_link_libraries(app PRIVATE sudoku)

# Copy assets
add_custom_command(
    TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:app>/assets
)

# Tests
if (${PREFIX}_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
