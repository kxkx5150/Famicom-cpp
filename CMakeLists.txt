cmake_minimum_required(VERSION 3.12)
project(Famicom-cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/exe)

file(GLOB sourcefiles 
    "*.h" "*.cpp" "NES/*.h" "NES/*.cpp" 
    "NES/apu/*.h" "NES/apu/*.cpp"
    )
add_executable(${PROJECT_NAME} ${sourcefiles})

find_package(OpenGL)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} SDL2_image SDL2 SDL2main)
