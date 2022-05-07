cmake_minimum_required(VERSION 3.12)
project(Famicom-cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../exe")

include_directories(./include)
link_directories(./lib)

file(GLOB sourcefiles "*.h" "*.cpp")
add_executable(${PROJECT_NAME} ${sourcefiles} 
    NES/nes.cpp NES/nes.h 
    NES/rom.cpp NES/rom.h
    NES/cpu.cpp NES/cpu.h
    NES/mem.cpp NES/mem.h
    NES/cpu_enum.h
    NES/mapper0.cpp NES/mapper0.h
    NES/ppu.cpp NES/ppu_port.cpp NES/ppu.h 
    NES/mapper.h
    NES/irq.h
    NES/dma.h
    NES/nestest.h
    )

find_package(OpenGL)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} SDL2_image SDL2 SDL2main)