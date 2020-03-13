#ifndef vtkSU2eader_h
#define vtkSU2Reader_h

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkObjectFactory.h"

class vtkSU2Reader : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkSU2Reader* New();
  vtkTypeMacro(vtkSU2Reader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetStringMacro(MeshFile);

  vtkGetStringMacro(MeshFile);

protected:
  vtkSU2Reader();
  ~vtkSU2Reader() override;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
private:
  char* MeshFile;
  vtkSU2Reader(const vtkSU2Reader&) = delete;
  void operator=(const vtkSU2Reader&) = delete;
};

#endif
