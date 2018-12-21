cmake_minimum_required(VERSION 3.13)
project(ASremote)

set(CMAKE_CXX_STANDARD 14)

include_directories(/usr/local/Cellar/ffmpeg/4.0.3/include/)
link_directories(/usr/local/Cellar/ffmpeg/4.0.3/lib/)

include_directories(/usr/local/Cellar/sdl2/2.0.8/include/)
link_directories(/usr/local/Cellar/sdl2/2.0.8/lib/)

add_executable(ASremote main.cpp SDL_Screen.cpp SDL_Screen.h SocketConnection.cpp SocketConnection.h FFmpegDecoder.cpp FFmpegDecoder.h FrameCache.cpp FrameCache.h EventCache.cpp EventCache.h EventController.cpp EventController.h)

target_link_libraries(
        ASremote
        avcodec
        avdevice
        avfilter
        avformat
        avresample
        avutil
        postproc
        swresample
        swscale
        SDL2
)