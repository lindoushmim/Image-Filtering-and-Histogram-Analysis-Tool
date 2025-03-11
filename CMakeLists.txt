cmake_minimum_required(VERSION 3.10)
project(MonProjet)

option(USE_GUI "Use the GUI main.cpp" ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)
find_package(wxWidgets REQUIRED)

include(${wxWidgets_USE_FILE})
include_directories(${CMAKE_SOURCE_DIR}/include)

if(USE_GUI)
    set(MAIN_SOURCE ${CMAKE_SOURCE_DIR}/gui/main.cpp)
else()
    set(MAIN_SOURCE ${CMAKE_SOURCE_DIR}/src/main.cpp)
endif()

set(SOURCES
    ${MAIN_SOURCE}
    ${CMAKE_SOURCE_DIR}/src/base.cpp
    ${CMAKE_SOURCE_DIR}/src/filter.cpp
    ${CMAKE_SOURCE_DIR}/src/geometric.cpp
    ${CMAKE_SOURCE_DIR}/src/histogram.cpp
)

add_executable(MonProjet ${SOURCES})

target_link_libraries(MonProjet ${OpenCV_LIBS} ${wxWidgets_LIBRARIES})
