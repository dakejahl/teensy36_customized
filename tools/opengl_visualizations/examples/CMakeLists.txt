#
# **NOTE** This example currently requires the LibSerial be built using
# autotools as the CMake build of LibSerial does not install a pkg-config
# file yet. Support for pkg-config will be added to CMake build shortly.
#
cmake_minimum_required(VERSION 3.8)

project(demo LANGUAGES C CXX VERSION 1.0.0)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


#
# Prefer -pthread compiler and linker flag when using libpthread. This must
# be set before call to find_package(Threads).
#
set(THREADS_HAVE_PTHREAD_ARG 1)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

#
# If you've installed LibSerial in a non-standard location, please add it
# to PKG_CONFIG_PATH first. For example, if LibSerial is installed under
# /opt/libserial, set PKG_CONFIG_PATH environment variable to
# /opt/libserial/lib/pkgconfig/ before running cmake for this example.
#
# export PKG_CONFIG_PATH=/opt/libserial/lib/pkgconfig/
#
find_package(PkgConfig)
pkg_check_modules(SERIAL libserial)

SET(CMAKE_EXE_LINKER_FLAGS
          "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/usr/local/lib")

add_executable(demo demo.cpp)

target_include_directories(demo PRIVATE
	serial_INCLUDE_DIRS
	${OPENGL_INCLUDE_DIRS}
	${GLUT_INCLUDE_DIRS}
)

target_link_libraries(demo PRIVATE
	serial
	${CMAKE_THREAD_LIBS_INIT}
	 ${OPENGL_LIBRARIES}
	${GLUT_LIBRARY}
)
