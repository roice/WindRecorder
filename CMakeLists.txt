#====================================================
# CMakeLists configuration file for WindRecorder
# 
# Author: Roice(LUO Bing)
# Date:   
#         2017-04-15 Create this file
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

#====================================================
# ================ User can Access ==================
#====================================================
# Project name
set(PRJ_NAME WindRecorder)
# Debug version
set(CMAKE_BUILD_TYPE Debug)
# ===================================================

#====================================================
# ============= Don't touch the following ===========
#====================================================
project(${PRJ_NAME})
#======== Detect which platform in use ========
# if compile on Mac OSX, the "-framework Cocoa" flag should be added
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework GLUT")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework GLUT")
    message("Compile on APPLE MACOSX")
elseif(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    message("Compile on UNIX")
endif()

#$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
#============== Find Dependencies ================
#$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
# find the FLTK package
find_package(FLTK REQUIRED)
find_package(OpenGL REQUIRED)
#---- External HDF5 related, for data recording
find_package(HDF5 COMPONENTS C CXX REQUIRED)
include_directories(${HDF5_INCLUDE_DIRS})

#$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
#============== Build the project ================
#$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
#---- source files include path settings ----
# add source directory into include path
#   add root dir to search for header files
include_directories(${PROJECT_SOURCE_DIR}/src)
#   add io dir
include_directories(${PROJECT_SOURCE_DIR}/src/io)

#---- start compiling ----
set(LIB_UI_NAME ui)
# make a library from ui files
add_library(${LIB_UI_NAME} src/ui/UI.cxx src/ui/View.cxx src/ui/agv.cxx
    src/ui/draw/DrawScene.cxx src/ui/draw/materials.cxx
    src/ui/draw/draw_arena.cxx) #src/ui/draw/draw_robots.cxx
#    src/ui/draw/draw_qr.cxx src/ui/draw/draw_wave.cxx
#    src/ui/draw/draw_arrow.cxx src/ui/draw/draw_wind.cxx
    # 3rdparty fltk widgets
    #    src/ui/widgets/Fl_LED_Button/Fl_LED_Button.cxx)
# compile main file
add_executable(${PRJ_NAME} src/main.cxx src/WR_config.cxx
    src/io/serial.cxx src/io/serial_anemometers.cxx src/io/serial_gill.cxx
    src/io/record.cxx)
target_compile_features(${PRJ_NAME} PRIVATE cxx_constexpr)
add_dependencies(${PRJ_NAME} ${LIB_UI_NAME})

#---- start linking ----
# Note: the former line depends on the next line
# link GUI library created above
target_link_libraries(${PRJ_NAME} ${LIB_UI_NAME})
# link external FLTK and OpenGL library
TARGET_LINK_LIBRARIES(${PRJ_NAME} ${FLTK_LIBRARIES})
TARGET_LINK_LIBRARIES(${PRJ_NAME} ${OPENGL_LIBRARIES})
# link external pthread library
target_link_libraries(${PRJ_NAME} pthread)
# link extern hdf5 library
target_link_libraries(${PRJ_NAME} ${HDF5_LIBRARIES})
