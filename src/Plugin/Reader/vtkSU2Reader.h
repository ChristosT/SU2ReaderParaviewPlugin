#ifndef vtkSU2eader_h
#define vtkSU2Reader_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkObjectFactory.h"

class vtkSU2Reader : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkSU2Reader* New();
  vtkTypeMacro(vtkSU2Reader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  vtkSetStringMacro(MeshFile);

  vtkGetStringMacro(MeshFile);

protected:
  vtkSU2Reader();
  ~vtkSU2Reader() VTK_OVERRIDE;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;
private:
  char* MeshFile;
  vtkSU2Reader(const vtkSU2Reader&) VTK_DELETE_FUNCTION;
  void operator=(const vtkSU2Reader&) VTK_DELETE_FUNCTION;
};

#endif
