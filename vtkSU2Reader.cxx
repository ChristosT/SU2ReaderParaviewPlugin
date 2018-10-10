#include "vtkSU2Reader.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"

#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkSmartPointer.h"

#include "SU2_mesh_io.h"

#include "vtkCellType.h"
#include "vtkCellData.h"
#include <vtkTypeInt64Array.h>

vtkStandardNewMacro(vtkSU2Reader);

// Construct object with merging set to true.
vtkSU2Reader::vtkSU2Reader()
{
    this->MeshFile = NULL;

    this->SetNumberOfInputPorts(0);
}


vtkSU2Reader::~vtkSU2Reader()
{
    delete [] this->MeshFile;
}

int vtkSU2Reader::RequestData( vtkInformation *vtkNotUsed(request), 
                                 vtkInformationVector **vtkNotUsed(inputVector), 
                                 vtkInformationVector *outputVector)
{
    // get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the ouptut
    vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT())); 

    if (!this->MeshFile)
    {
        vtkErrorMacro(<<"A File Name for the Mesh must be specified.");
        return 0;
    }

    // open a SU2 file for reading
    SU2_MESH_IO::SU2_mesh mesh;
    bool success = SU2_MESH_IO::open_mesh(this->MeshFile,SU2_MESH_IO::file_mode::READ,mesh);
    if(not success)
    {
        vtkWarningMacro(<<"Could not open SU2 file");
        return 0;
    }

    vtkDebugMacro(<<"Mesh File Opened");
    
    // Read the number of vertices
    const int64_t numVerts = SU2_MESH_IO::stat_kwd(mesh,SU2_MESH_IO::NPOIN);

    // Set up point container
    vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
    pts->SetDataTypeToDouble();
    pts->SetNumberOfPoints(numVerts);

    double p[3];
    int64_t rt; //reference , not in use in this plugin

    // Read the vertices
    //GmfGotoKwd(InpMsh, GmfVertices);
    int dim = mesh.dim;
    if( dim ==3)
    {
        for(int64_t i=0;i<numVerts;i++)
        {
            SU2_MESH_IO::get_line(mesh, SU2_MESH_IO::POINT3D, p[0], p[1], p[2],rt);
            vtkDebugMacro(<<p[0] << " " <<p[1] << " " << p[2]);
            pts->SetPoint(i,p);
        }
    }
    else
    {
        p[2] = 0.0;
        for(int64_t i=0;i<numVerts;i++)
        {
            SU2_MESH_IO::get_line(mesh, SU2_MESH_IO::POINT2D, p[0], p[1],rt);
            vtkDebugMacro(<<p[0] << " " <<p[1] << " " << p[2]);
            pts->SetPoint(i,p);
        }
    }
    output->SetPoints(pts);
    
    // Read elements
    const int64_t numElements = SU2_MESH_IO::stat_kwd(mesh,SU2_MESH_IO::NELEM);
    output->Allocate(numElements);

    SU2_MESH_IO::SU2Keyword type;
    vtkIdType v[8];
    int64_t v0,v1,v2,v3,v4,v5,v6,v7;
    //LINE = 3,
    //TRIANGLE = 5,
    //QUADRILATERAL = 9,
    //TETRAHEDRON = 10,
    //HEXAHEDRON = 12,
    //PRISM = 13,
    //PYRAMID = 14
    for (int64_t i=0; i < numElements; i++)
    {
        type = SU2_MESH_IO::get_element_type(mesh);
        switch(type)
        {
            case SU2_MESH_IO::SU2Keyword::LINE:
                //SU2_MESH_IO::get_line(mesh,type,v[0],v[1],rt); 
                //output->InsertNextCell(VTK_LINE,2,v);
                break;
            case SU2_MESH_IO::SU2Keyword::TRIANGLE:
                SU2_MESH_IO::get_line(mesh,type,v0,v1,v2,rt); 
                v[0] = v0;
                v[1] = v1;
                v[2] = v2;
                output->InsertNextCell(VTK_TRIANGLE,3,v);
                break;
            case SU2_MESH_IO::SU2Keyword::QUADRILATERAL:
                SU2_MESH_IO::get_line(mesh,type,v0,v1,v2,v3,rt); 
                v[0] = v0;
                v[1] = v1;
                v[2] = v2;
                v[3] = v3;
                output->InsertNextCell(VTK_QUAD,4,v);
                break;
            case SU2_MESH_IO::SU2Keyword::TETRAHEDRON:
                SU2_MESH_IO::get_line(mesh,type,v0,v1,v2,v3,rt); 
                v[0] = v0;
                v[1] = v1;
                v[2] = v2;
                v[3] = v3;
                output->InsertNextCell(VTK_TETRA,4,v);
                break;
            case SU2_MESH_IO::SU2Keyword::PRISM:
                SU2_MESH_IO::get_line(mesh,type,v0,v1,v2,v3,v4,v5,rt); 
                v[0] = v0;
                v[1] = v1;
                v[2] = v2;
                v[3] = v3;
                v[4] = v4;
                v[5] = v5;
                output->InsertNextCell(VTK_WEDGE,6,v);
                break;
            case SU2_MESH_IO::SU2Keyword::PYRAMID:
                SU2_MESH_IO::get_line(mesh,type,v0,v1,v2,v3,v4,rt); 
                v[0] = v0;
                v[1] = v1;
                v[2] = v2;
                v[3] = v3;
                v[4] = v4;
                output->InsertNextCell(VTK_PYRAMID,6,v);
                break;
            case SU2_MESH_IO::SU2Keyword::HEXAHEDRON:
                SU2_MESH_IO::get_line(mesh,type,v0,v1,v2,v3,v4,v5,v6,v7,rt); 
                v[0] = v0;
                v[1] = v1;
                v[2] = v2;
                v[3] = v3;
                v[4] = v4;
                v[5] = v5;
                v[6] = v6;
                v[7] = v7;
                output->InsertNextCell(VTK_HEXAHEDRON,8,v);
                break;
            default:
                vtkWarningMacro(<<"Unknown element type");

        }
    }


    output->Squeeze();

    return 1;
}

void vtkSU2Reader::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os  << indent << "MeshFile: "
        << (this->MeshFile ? this->MeshFile : "(none)") << "\n";
}
