set(classes
    vtkSU2Reader
    vtkSU2ReaderInternal
    )

vtk_module_add_module(SU2Reader
  CLASSES ${classes}
  )
paraview_add_server_manager_xmls(
    XMLS  vtkSU2Reader.xml)
