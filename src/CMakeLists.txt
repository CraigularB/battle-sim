cmake_minimum_required(VERSION 2.4)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -Werror -D_GNU_SOURCE=1 -Dmain=SDL_main -O2 -g -DBOOST_FILESYSTEM_NO_DEPRECATED -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_THREAD_PROVIDES_FUTURE -DBOOST_THREAD_USE_LIB")

set(EXENAME battle)

include_directories("C:/Libraries/SDL-1.2.15/include/SDL"
    "C:/Libraries/SDL_image-1.2.12/include"
    "C:/Libraries/SDL_mixer-1.2.12/include"
    "C:/Libraries/SDL_ttf-2.0.11/include"
    "C:/Libraries/SDL_gfx-2.0.24"
    "C:/Libraries/rapidjson-0.11/include")

include_directories(SYSTEM "C:/Boost/include/boost-1_55")

# Must appear before the add_executable line.
link_directories("C:/Libraries/SDL-1.2.15/lib"
    "C:/Libraries/SDL_image-1.2.12/lib/x86"
    "C:/Libraries/SDL_mixer-1.2.12/lib/x86"
    "C:/Libraries/SDL_ttf-2.0.11/lib/x86"
    "C:/Boost/lib")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")

file(GLOB SRC *.cpp)
set(SRC ${SRC} "C:/Libraries/SDL_gfx-2.0.24/SDL_rotozoom.c")
add_executable(${EXENAME} ${SRC})

# Must appear after add_executable line.
target_link_libraries(${EXENAME} mingw32 SDLmain SDL SDL_image SDL_ttf
    SDL_mixer boost_thread-mgw48-mt-s-1_55 boost_filesystem-mgw48-s-1_55
    boost_system-mgw48-s-1_55)
