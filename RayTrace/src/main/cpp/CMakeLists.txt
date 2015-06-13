cmake_minimum_required (VERSION 2.8)

find_package(GLUT)
find_package(OpenGL)
include_directories( ${GLUT_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

add_executable( RayTrace /
    3ds.cpp /
    starter.cpp /
    xmlParser.cpp /
    NormalRenderer.cpp /
    Ray.cpp /
    Scene.cpp)

target_link_libraries( RayTrace ${GLUT_glut_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
