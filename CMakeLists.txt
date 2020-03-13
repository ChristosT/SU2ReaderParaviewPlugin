# create a plugin that adds a reader to the ParaView GUI
# it is added in the file dialog when doing open

cmake_minimum_required(VERSION 2.8)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

find_package(ParaView REQUIRED)
include(${PARAVIEW_USE_FILE})
set(SU2_READER_LIB_NAME vtkSU2Reader)
ADD_PARAVIEW_PLUGIN(${SU2_READER_LIB_NAME} "1.0"
  SERVER_MANAGER_XML vtkSU2Reader.xml
  SERVER_MANAGER_SOURCES vtkSU2Reader.cxx
  REQUIRED_ON_SERVER)

###    LIBMESH Library   ###
find_path(SU2_MESH_IO_INCLUDE_DIR SU2_mesh_io.h HINTS .) #"${HINT_PATH}/include/")
if(NOT SU2_MESH_IO_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find SU2_mesh_io.h . Please set SU2_MESH_IO_INCLUDE_DIR")
endif()

include_directories( ${SU2_MESH_IO_INCLUDE_DIR})
find_library(SU2_MESH_IO_LIBRARY SU2_mesh_io HINT .) #"${HINT_PATH}/lib")
if(NOT SU2_MESH_IO_LIBRARY)
    message(FATAL_ERROR "Could not find SU2_mesh_io lib.  Please set SU2_MESH_IO_LIBRARY")
endif()


target_link_libraries(${SU2_READER_LIB_NAME} PUBLIC ${SU2_MESH_IO_LIBRARY} ${VTK_LIBRARIES})
