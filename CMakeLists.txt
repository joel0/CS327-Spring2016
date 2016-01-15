#I do not know what I am doing.  This file seems to work.

cmake_minimum_required(VERSION 3.3)
#project(project0)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-std=c11)
include_directories(project0/include)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

set(SOURCE_FILES )
set(PROJECT0.0 project0/src/hyperbinary.c)
set(PROJECT0.1 project0/src/rationalNumber.c)
#set(PROJECT1 project1.c)
add_executable(project0.0 ${SOURCE_FILES} ${PROJECT0.0})
add_executable(project0.1 ${SOURCE_FILES} ${PROJECT0.1})
#add_executable(project1 ${SOURCE_FILES} ${PROJECT1})