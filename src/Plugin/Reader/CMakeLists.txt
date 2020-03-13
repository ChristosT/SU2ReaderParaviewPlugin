set(classes
    vtkSU2Reader)

vtk_module_add_module(SU2Reader
  CLASSES ${classes}
  )
vtk_module_link(SU2Reader PRIVATE SU2_mesh_io)
paraview_add_server_manager_xmls(
    XMLS  vtkSU2Reader.xml)
