cmake_minimum_required(VERSION 3.10)
project(MinecraftPE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_OPENGL_ES "Use OpenGL ES instead of desktop OpenGL" OFF)
option(BUILD_TOUCH "Build touch screen GUI" OFF)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type: Debug or Release")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")

add_definitions(-DLINUX)

if(USE_OPENGL_ES)
    add_definitions(-DOPENGL_ES)
endif()

include_directories(src)
include_directories(libraries/include)
include_directories(src/raknet)
include_directories(/usr/include/GL)
include_directories(/usr/include/AL)
include_directories(/usr/include/gl)

if(USE_OPENGL_ES)
    add_definitions(-DOPENGL_ES)
endif()

include_directories(src)
include_directories(libraries/include)
include_directories(src/raknet)

set(COMMON_SOURCES
    src/NinecraftApp.cpp
    src/Performance.cpp
    src/SharedConstants.cpp
    src/client/Minecraft.cpp
    src/client/Options.cpp
    src/client/OptionsFile.cpp
    src/client/OptionStrings.cpp
    src/client/IConfigListener.cpp
    src/client/MouseHandler.cpp
    src/client/player/LocalPlayer.cpp
    src/client/player/RemotePlayer.cpp
    src/client/player/input/KeyboardInput.cpp
    src/locale/I18n.cpp
    src/nbt/Tag.cpp
    src/network/ClientSideNetworkHandler.cpp
    src/network/NATPunchHandler.cpp
    src/network/NetEventCallback.cpp
    src/network/Packet.cpp
    src/network/PHPDirectoryServer2.cpp
    src/network/RakNetInstance.cpp
    src/network/ServerSideNetworkHandler.cpp
    src/network/command/CommandServer.cpp
    src/platform/CThread.cpp
    src/platform/time.cpp
    src/platform/input/Controller.cpp
    src/platform/input/Keyboard.cpp
    src/platform/input/Mouse.cpp
    src/platform/input/Multitouch.cpp
    src/util/DataIO.cpp
    src/util/Mth.cpp
    src/util/PerfTimer.cpp
    src/util/StringUtils.cpp
)

file(GLOB_RECURSE SERVER_FILES "src/server/*.cpp")
file(GLOB_RECURSE WORLD_FILES "src/world/*.cpp")
file(GLOB_RECURSE RAKNET_FILES "src/raknet/*.cpp")
file(GLOB_RECURSE CLIENT_RENDERER_FILES "src/client/renderer/*.cpp")
file(GLOB_RECURSE CLIENT_PARTICLE_FILES "src/client/particle/*.cpp")
file(GLOB_RECURSE CLIENT_MODEL_FILES "src/client/model/*.cpp")
file(GLOB_RECURSE CLIENT_MODEL_GEOM_FILES "src/client/model/geom/*.cpp")
file(GLOB_RECURSE CLIENT_GUI_FILES "src/client/gui/*.cpp")
file(GLOB_RECURSE CLIENT_GUI_COMPONENTS_FILES "src/client/gui/components/*.cpp")
file(GLOB_RECURSE CLIENT_GUI_SCREENS_FILES "src/client/gui/screens/*.cpp")
file(GLOB_RECURSE CLIENT_GUI_CRAFTING_FILES "src/client/gui/screens/crafting/*.cpp")
file(GLOB_RECURSE CLIENT_GUI_TOUCH_FILES "src/client/gui/screens/touch/*.cpp")
file(GLOB_RECURSE CLIENT_SOUND_FILES "src/client/sound/*.cpp")

list(REMOVE_ITEM COMMON_SOURCES src/raknet/VitaIncludes.cpp)
list(REMOVE_ITEM COMMON_SOURCES src/raknet/Rackspace.cpp)
list(REMOVE_ITEM COMMON_SOURCES src/raknet/PS3Includes.cpp)
list(REMOVE_ITEM COMMON_SOURCES src/raknet/CloudCommon.cpp)
list(REMOVE_ITEM COMMON_SOURCES src/raknet/CloudClient.cpp)
list(REMOVE_ITEM COMMON_SOURCES src/raknet/CloudServer.cpp)
list(REMOVE_ITEM COMMON_SOURCES src/raknet/EmailSender.cpp)
list(REMOVE_ITEM COMMON_SOURCES src/raknet/Autopatcher.cpp)
list(REMOVE_ITEM COMMON_SOURCES src/raknet/AutopatcherServer.cpp)
list(REMOVE_ITEM COMMON_SOURCES src/raknet/AutopatcherClient.cpp)
list(REMOVE_ITEM COMMON_SOURCES src/raknet/Rackspace.cpp)

# Always include touch screens for Linux (they work with mouse too)
if(LINUX)
    # Keep touch screens - they have better styled UI
else()
if(NOT BUILD_TOUCH)
    list(REMOVE_ITEM CLIENT_GUI_SCREENS_FILES ${CLIENT_GUI_TOUCH_FILES})
    list(REMOVE_ITEM CLIENT_SOURCES ${CLIENT_GUI_TOUCH_FILES})
endif()
endif()

set(PLATFORM_AUDIO_FILES
    src/platform/audio/SoundSystemAL.cpp
)

find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

find_package(OpenGL)
find_package(X11)
find_package(SDL2 QUIET)
find_package(OpenAL QUIET)
if(NOT OPENAL_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OPENAL REQUIRED openal)
endif()

set(SERVER_SOURCES
    src/main_dedicated.cpp
    ${COMMON_SOURCES}
    ${SERVER_FILES}
    ${WORLD_FILES}
    ${RAKNET_FILES}
)

set(CLIENT_SOURCES
    src/main_linux.cpp
    ${COMMON_SOURCES}
    ${CLIENT_RENDERER_FILES}
    ${CLIENT_PARTICLE_FILES}
    ${CLIENT_MODEL_FILES}
    src/util/PerfRenderer.cpp
    ${CLIENT_MODEL_GEOM_FILES}
    ${CLIENT_GUI_FILES}
    ${CLIENT_GUI_COMPONENTS_FILES}
    ${CLIENT_GUI_SCREENS_FILES}
    ${CLIENT_GUI_CRAFTING_FILES}
    ${CLIENT_SOUND_FILES}
    ${PLATFORM_AUDIO_FILES}
    ${WORLD_FILES}
    ${RAKNET_FILES}
    src/server/ServerLevel.cpp
    src/server/ServerPlayer.cpp
    src/client/gamemode/CreativeMode.cpp
    src/client/gamemode/SurvivalMode.cpp
    src/client/gamemode/GameMode.cpp
    src/client/gamemode/CreatorMode.cpp
    src/client/player/input/touchscreen/TouchscreenInput.cpp
)

if(SDL2_FOUND)
    set(SDL2_LIBRARY SDL2::SDL2)
    set(SDL2_INCLUDE_DIR ${SDL2_INCLUDE_DIRS})
else()
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2 sdl2)
    endif()
endif()

if(OPENGL_FOUND)
    set(OPENGL_LIBRARY ${OPENGL_gl_LIBRARY} ${OPENGL_GLU_LIBRARY})
else()
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_check_modules(OPENGL gl)
    endif()
endif()

find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(GLEW glew)
endif()

add_definitions(-DGLEW_STATIC)

if(GLEW_FOUND)
    if(GLEW_STATIC)
        set(GLEW_LIBRARIES ${GLEW_LIBRARIES} -lGLEW)
    endif()
endif()

if(X11_FOUND)
    set(X11_LIBRARY ${X11_LIBRARIES})
    set(X11_INCLUDE_DIR ${X11_INCLUDE_DIR})
endif()

add_executable(MinecraftServer ${SERVER_SOURCES})
target_compile_definitions(MinecraftServer PRIVATE STANDALONE_SERVER)
target_link_libraries(MinecraftServer ZLIB::ZLIB ${CMAKE_THREAD_LIBS_INIT} dl)

add_executable(MinecraftClient ${CLIENT_SOURCES})

if(NOT USE_OPENGL_ES)
    target_compile_definitions(MinecraftClient PRIVATE NO_EGL)
endif()

if(SDL2_FOUND)
    target_include_directories(MinecraftClient PRIVATE ${SDL2_INCLUDE_DIR})
    target_link_libraries(MinecraftClient ${SDL2_LIBRARY})
endif()

if(OPENGL_FOUND)
    target_link_libraries(MinecraftClient ${OPENGL_LIBRARY})
endif()

if(GLEW_FOUND)
    target_include_directories(MinecraftClient PRIVATE ${GLEW_INCLUDE_DIRS})
    target_link_libraries(MinecraftClient ${GLEW_LIBRARIES})
endif()

if(USE_OPENGL_ES)
    target_link_libraries(MinecraftClient EGL)
endif()

if(X11_FOUND)
    target_include_directories(MinecraftClient PRIVATE ${X11_INCLUDE_DIR})
    target_link_libraries(MinecraftClient ${X11_LIBRARY} pthread)
endif()

target_link_libraries(MinecraftClient ZLIB::ZLIB ${CMAKE_THREAD_LIBS_INIT} dl png)

if(OPENAL_FOUND)
    target_include_directories(MinecraftClient PRIVATE ${OPENAL_INCLUDE_DIR})
    target_link_libraries(MinecraftClient ${OPENAL_LIBRARY})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_BUILD)
    if(UNIX AND NOT APPLE)
        add_definitions(-DLINUX_LOG_FILE)
    endif()
endif()

if(MSVC)
    target_compile_options(MinecraftServer PRIVATE /utf-8)
    target_compile_options(MinecraftClient PRIVATE /utf-8)
else()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(MinecraftServer PRIVATE -Wno-write-strings -fpermissive -g -O0)
        target_compile_options(MinecraftClient PRIVATE -Wno-write-strings -fpermissive -g -O0)
    else()
        target_compile_options(MinecraftServer PRIVATE -Wno-write-strings -fpermissive -w -O3)
        target_compile_options(MinecraftClient PRIVATE -Wno-write-strings -fpermissive -w -O3)
    endif()
endif()
