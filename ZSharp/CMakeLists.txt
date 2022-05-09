cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ZSharp)

SET(programName Slang)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeFiles/")

add_executable (ZSharp Main.cpp main.h anyops.h builtin.h strops.cpp strops.h eval.cpp eval.h graphics.h ZS.h color.hpp)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
# include_directories("D:/Code/SDL2_ttf-2.0.15/include")
# include_directories("D:/Code/SDL2-2.0.18/include")
# include_directories("D:/Code/SDL2_image-2.0.5/include")
# include_directories("D:/Code/boost")

# link_directories("D:/Code/SDL2_ttf-2.0.15/lib/x64")
# link_directories("D:/Code/SDL2-2.0.18/lib/x64")
# link_directories("D:/Code/SDL2_image-2.0.5/lib/x64")
# link_directories("D:/Code/boost")

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
FIND_PACKAGE(Boost REQUIRED COMPONENTS system) 

include_directories(${Boost_INCLUDE_DIRS}) 

target_link_libraries(ZSharp m ${Boost_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})
