#include "SU2_mesh_io.h"
#include <iostream>

int main(int argc, char** argv)
{
    SU2_MESH_IO::SU2_mesh mesh;
    std::string line;
    bool ans = SU2_MESH_IO::open_mesh(argv[1],SU2_MESH_IO::file_mode::READ,mesh);
    if( not ans)
    {
        std::cerr<< "Error Opening the file" << "\n";
        return 1;
    }
    std::cout << "ans=" << ans << std::endl;

    int k =  SU2_MESH_IO::stat_kwd(mesh,SU2_MESH_IO::NPOIN);
    if( k==-1)
    {
        std::cerr<< "Error in reading points" << "\n";
        return 1;
    }
    std::cout << "number of points " << k << std::endl;
    double x,y,z;
    for(int j = 0 ; j < k ; j++)
    {
        SU2_MESH_IO::get_line(mesh,SU2_MESH_IO::POINT2D,x,y);
        std::cout << __func__ << ":" << x << " " << y << " " << std::endl;
    }
    
    k =  SU2_MESH_IO::stat_kwd(mesh,SU2_MESH_IO::NELEM);
    if( k==-1)
    {
        std::cerr<< "Error in readinng points" << "\n";
        return 1;
    }
    SU2_MESH_IO::SU2Keyword type;
    uint64_t a,b,c,d;
    for(int j = 0 ; j < k ; j++)
    {
        //get_line(mesh,{a,b,c,ref});
        type = SU2_MESH_IO::get_element_type(mesh);
        SU2_MESH_IO::get_line(mesh,type,a,b,c,d);
        std::cout << type << " " << a << " " << b << " " << c <<" "  << d << std::endl;
    }

    std::vector< std::string> labels = SU2_MESH_IO::get_marker_tags(mesh);

    for(std::string& key : labels)
    {
        k = SU2_MESH_IO::stat_kwd(mesh,key);
        std::cout << "Label " << key << " elements : " << k << std::endl;
        
        for(int j = 0 ; j < k ; j++)
        {
            type = SU2_MESH_IO::get_element_type(mesh);
            SU2_MESH_IO::get_line(mesh,type,a,b);
            std::cout << type << " " << a << " " << b << std::endl;
        }
    }


    return 0;
}
