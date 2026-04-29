cmake_minimum_required(VERSION 3.20)

set(TEMPLATE_PROJECT_NAME "mobile-game-template")
set(TEMPLATE_DISPLAY_NAME "Mobile Game Template")
set(TEMPLATE_ORIENTATION "portrait")
set(TEMPLATE_ORIENTATION_VALUE "1")
if(EXISTS "${CMAKE_SOURCE_DIR}/template.ini")
    file(STRINGS "${CMAKE_SOURCE_DIR}/template.ini" TEMPLATE_CONFIG_LINES)
    foreach(CONFIG_LINE IN LISTS TEMPLATE_CONFIG_LINES)
        if(CONFIG_LINE MATCHES "^[ \t]*project_name[ \t]*=[ \t]*(.+)$")
            set(TEMPLATE_PROJECT_NAME "${CMAKE_MATCH_1}")
            string(STRIP "${TEMPLATE_PROJECT_NAME}" TEMPLATE_PROJECT_NAME)
        elseif(CONFIG_LINE MATCHES "^[ \t]*display_name[ \t]*=[ \t]*(.+)$")
            set(TEMPLATE_DISPLAY_NAME "${CMAKE_MATCH_1}")
            string(STRIP "${TEMPLATE_DISPLAY_NAME}" TEMPLATE_DISPLAY_NAME)
        elseif(CONFIG_LINE MATCHES "^[ \t]*orientation[ \t]*=[ \t]*(.+)$")
            set(TEMPLATE_ORIENTATION "${CMAKE_MATCH_1}")
            string(STRIP "${TEMPLATE_ORIENTATION}" TEMPLATE_ORIENTATION)
        endif()
    endforeach()
endif()

string(TOLOWER "${TEMPLATE_ORIENTATION}" TEMPLATE_ORIENTATION)
if(TEMPLATE_ORIENTATION STREQUAL "landscape")
    set(TEMPLATE_ORIENTATION_VALUE "2")
endif()

project(${TEMPLATE_PROJECT_NAME} C)

set(CMAKE_C_STANDARD 11)

set(GAME_SOURCES
    src/main.c
    src/app.c
    src/game.c
    src/input.c
    src/player.c
    src/render.c
)

add_executable(${PROJECT_NAME} ${GAME_SOURCES})

find_package(raylib CONFIG REQUIRED)
find_package(flecs CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    $<IF:$<TARGET_EXISTS:flecs::flecs>,flecs::flecs,flecs::flecs_static>
)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    APP_DISPLAY_NAME="${TEMPLATE_DISPLAY_NAME}"
    APP_ORIENTATION=${TEMPLATE_ORIENTATION_VALUE}
)
