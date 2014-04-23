cmake_minimum_required (VERSION 2.8)

find_package(GLUT)
include_directories( ${GLUT_INCLUDE_DIRS} )

add_executable( RayTrace 3ds.cpp starter.cpp xmlParser.cpp)
