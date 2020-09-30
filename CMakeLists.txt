cmake_minimum_required(VERSION 3.5)  # CMake version check
project(upfw)
set(CMAKE_CXX_STANDARD 17)            # Enable c++17 standard

include_directories(/usr/local/include)
link_directories(/usr/local/lib)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

message(STATUS "system: ${CMAKE_SYSTEM_PREFIX_PATH}")

add_executable(upfw upfw.cpp Processor.cpp Connector.cpp)

target_link_libraries(upfw PRIVATE restclient-cpp.a PRIVATE libcurl.a PRIVATE nlohmann_json::nlohmann_json PRIVATE CLI11::CLI11)
