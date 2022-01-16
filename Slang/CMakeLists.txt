cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Slang)

# Above line indicates to Cmake that minimum version 2.8 is required.
# As you may have noted all lines beginning with hash symbol are treated as comments by Cmake.
SET(programName Slang)
# In the above line we declared a variable programName and assigned it a value MyApp. We will make use of the same later.project(PrjMyApp)
# here we define the project or solution (for visual studio) name
# In following lines we try to find out the packages of additional libraries, if reqd. Here we are trying to locate PCL and mrpt library.
# u may replace the names by yours. Each such package should have a Cmake file in them, if not then we may have to manually define the paths.
# we Show that later.
# find_package(PCL 1.2 REQUIRED)
# FIND_PACKAGE(MRPT REQUIRED base;bayes;obs;gui)

# here we specify the additional include directories for the project. These files come in additional include directories option of VC++
# project.
# either the variable values from package like ${PCL_INCLUDE_DIRS} or absolute paths as shown in second and third line may be used.
#include_directories(${PCL_INCLUDE_DIRS})
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
FIND_PACKAGE(Boost REQUIRED COMPONENTS system) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable(progname file1.cxx file2.cxx) 
    target_link_libraries(progname ${Boost_LIBRARIES})
endif()

include_directories("D:/Code/SDL2_ttf-2.0.15/include")
include_directories("D:/Code/SDL2-2.0.18/include")
include_directories("D:/Code/SDL2_image-2.0.5/include")
include_directories("D:/Code/boost")

# here we specify the additional library directories for the linker in the project. These files come in additional library directories
# option of VC++ project.
# either the variable values from package like ${PCL_LIBRARY_DIRS} or absolute paths as shown in second and third line may be used.
# An update, link_directories is being pulled out, you may use TARGET_LINK_LIBRARIES instead as shown below
#link_directories(${PCL_LIBRARY_DIRS})
link_directories("D:/Code/SDL2_ttf-2.0.15/lib/x64")
link_directories("D:/Code/SDL2-2.0.18/lib/x64")
link_directories("D:/Code/SDL2_image-2.0.5/lib/x64")
link_directories("D:/Code/boost")

# here we add definitions for any package if requred.
#add_definitions(${PCL_DEFINITIONS})

# The following line is very important.
# It specifies the executable name. Here the executable is the first parameter i.e. Slang and a file Slang.exe will be created on
# compilation in windows platform.
# Followed by the executable name come all your source and header files.
# All cpp fles will be clubbed into source folder and all .h files will be added to header files folder of the project.
add_executable (Slang Main.cpp main.h anyops.h builtin.h strops.cpp strops.h eval.cpp eval.h graphics.h SLB.h)

target_include_directories (Slang "D:/Code/boost/")
# There may be some additional dependencies which you may have to specify for the project, which you may do as in following lines.
# Note that first parameter is the executable name.
target_link_libraries (Slang SDL2.lib SDL2main.lib SDL2_ttf.lib SDL2_image.lib)
# # Add the required libraries for linking:
# TARGET_LINK_LIBRARIES(${programName}
# ${MRPT_LIBS} # This is filled by FIND_PACKAGE(MRPT ...)
# "" # Optional extra libs...
# )