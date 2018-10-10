#include "SU2_mesh_io.h"
#include <string>
#include <cstring>
#include <iostream>
#include <functional>

namespace SU2_MESH_IO
{
#define DPRINT(x) std::cout << #x << " ---> "<< x << std::endl;
const int MAX_STRING_SIZE = 1024;

const char KeywordNames[NumKw][MAX_STRING_SIZE] =
{
    "NDIME",
    "NELEM",
    "NPOIN",
    "NMARK"
};


/*
 * Scan the whole file and find the locations of file keywords
 */
static bool find_keyword_locations(SU2_mesh& mesh)
{

    std::string line;

    while(std::getline(mesh.file,line))
    {
         // Check whether the line start by a character
         if(isalpha(line[0]))
         {
             for(int i = 0 ; i <  NumKw; i++)
             {
                 int res = std::strncmp(line.c_str(),KeywordNames[i], std::min(line.size(),std::strlen(KeywordNames[i])));
                 if( res == 0 )
                 {
                     //save beginning of line 
                     mesh.KwLocations[i] = (std::uint64_t)mesh.file.tellg() - line.size();
                     break;
                }
            }
        }
    }
    // clear failbit,a llow for more searches
    mesh.file.clear();
    return true;
}

bool open_mesh(const char* filename, enum file_mode fmode, SU2_mesh& mesh)
{

    if( fmode == file_mode::WRITE)
    {
        std::cerr<< " NOT YET IMPLEMENTED " << std::endl;
        return false;
    }
    else
    {
        mesh.fmode = fmode;
        mesh.file.open(filename,std::ios::in);

        if( not mesh.file.is_open())
        {
            std::cerr<< " Could not open " << filename << std::endl;
            return false;
        }

        find_keyword_locations(mesh);
        // save dimension
        mesh.dim = (int)stat_kwd( mesh, NDIME);

    }
    return true;
}
/**
 * Read number entities under kwd and set position to this kwd
 */
int64_t stat_kwd(SU2_mesh& mesh, int kwd)
{
    if ( kwd < 0 or kwd >=NumKw)
    {
        std::cerr<< "Wrong kwd value";
        return -1;
    }
    int64_t pos = mesh.KwLocations[kwd];

    // Go to the beginning of the line of kwd
    mesh.file.seekg( mesh.KwLocations[kwd] );
    // skip keyword 
    std::string tmp;
    mesh.file >> tmp;

    // read number of entities
    int64_t value;
    mesh.file >> value;

    return value;
}
//void get_line(SU2_mesh& mesh, std::initializer_list< std::reference_wrapper<int64_t>> parameters)
//{
//    for( auto& i : parameters)
//    {
//        mesh.file >> i.get();
//       DPRINT(i.get());
//    }
//}



//overload for points
void get_line(SU2_mesh& mesh, double& x, double& y, double& z, int64_t& index)
{
    mesh.file >> x >> y >> z >> index;
}
// 2D points
void get_line(SU2_mesh& mesh, double& x, double& y, int64_t& index)
{
    mesh.file >> x >> y >> index;
}

};
