cmake_minimum_required(VERSION 3.10)

# set the project name
project(Slang)

# add the executable
#add_executable(Main.cpp)

target_include_directories(Slang PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )