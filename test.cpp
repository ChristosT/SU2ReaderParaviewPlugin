#include "SU2_mesh_io.h"
#include <iostream>

int main(int argc, char** argv)
{
    SU2_MESH_IO::SU2_mesh mesh;
    std::string line;
    bool ans = SU2_MESH_IO::open_mesh(argv[1],SU2_MESH_IO::file_mode::READ,mesh);
    if( not ans)
            return 1;

    int k =  SU2_MESH_IO::stat_kwd(mesh,SU2_MESH_IO::NPOIN);
    double x,y,z;
    int64_t ref;
    for(int j = 0 ; j < k ; j++)
    {
        SU2_MESH_IO::get_line(mesh,SU2_MESH_IO::POINT2D,x,y,ref);
        std::cout << x << " " << y << " "  << ref << std::endl;
    }
    
    k =  SU2_MESH_IO::stat_kwd(mesh,SU2_MESH_IO::NELEM);
    SU2_MESH_IO::SU2Keyword type;
    int64_t a,b,c;
    for(int j = 0 ; j < k ; j++)
    {
        //get_line(mesh,{a,b,c,ref});
        type = SU2_MESH_IO::get_element_type(mesh);
        SU2_MESH_IO::get_line(mesh,type,a,b,c,ref);
        std::cout << type << a << " " << b << " " << c <<" "  << ref << std::endl;
    }
    return 0;
}
