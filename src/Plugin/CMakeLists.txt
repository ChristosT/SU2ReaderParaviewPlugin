paraview_add_plugin(SU2ReaderParaview
  VERSION "2.0"
  MODULES SU2Reader
  REQUIRED_ON_SERVER
  MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Reader/vtk.module")

