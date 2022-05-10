cmake_minimum_required(VERSION 3.12)
project(Famicom-cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../exe")

include_directories(./include)
link_directories(./lib)

file(GLOB sourcefiles 
    "*.h" "*.cpp" "NES/*.h" "NES/*.cpp" 
    "NES/apu/Nes_Snd_Emu-0.1.7/*.h" "NES/apu/Nes_Snd_Emu-0.1.7/*.cpp"
    "NES/apu/Nes_Snd_Emu-0.1.7/nes_apu/*.h" "NES/apu/Nes_Snd_Emu-0.1.7/nes_apu/*.cpp"
    )
add_executable(${PROJECT_NAME} ${sourcefiles})

find_package(OpenGL)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} SDL2_image SDL2 SDL2main)