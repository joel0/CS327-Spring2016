#I do not know what I am doing.  This file seems to work.

cmake_minimum_required(VERSION 3.3)
#project(project0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-std=c11)

set(SOURCE_FILES )
set(PROJECT0 project0/hyperbinary.c project0/hyperbinary.h)
#set(PROJECT1 project1.c)
add_executable(project0 ${SOURCE_FILES} ${PROJECT0})
#add_executable(project1 ${SOURCE_FILES} ${PROJECT1})