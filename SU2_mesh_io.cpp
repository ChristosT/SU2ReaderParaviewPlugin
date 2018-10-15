#include "SU2_mesh_io.h"
#include <string>
#include <cstring>
#include <iostream>
#include <functional>
#include <sstream>
#include <cassert>
#define DPRINT(x) std::cout <<__func__ << " : " << #x << " ---> "<< x << std::endl;

namespace SU2_MESH_IO
{
const int MAX_STRING_SIZE = 1024;

const char KeywordNames[NumKw][MAX_STRING_SIZE] =
{
    "NDIME",
    "NELEM",
    "NPOIN",
    "NMARK"
};

namespace detail
{
    // use strinstream instead of the string that std::getline returns
    // that way we get split on whitespace without extra eford
    static std::istringstream getline(std::fstream& file)
    {
        std::string line;
        std::getline(file, line);
        
        std::istringstream stream(line);
    
        return stream;
    }
        
}
/*
 * Scan the whole file and find the locations of file keywords
 */
static bool find_keyword_locations(SU2_mesh& mesh)
{

    std::string line;

    while(std::getline(mesh.file,line))
    {
         // Check whether the line starts by a character
         // of if it is a comment
         if(line.size() > 0 and (line[0] != '%') and (isalpha(line[0])) )
         {
             for(int i = 0 ; i <  NumKw; i++)
             {
                 int res = std::strncmp(line.c_str(),KeywordNames[i], std::min(line.size(),std::strlen(KeywordNames[i])));
                 if( res == 0 )
                 {
                     //save position of data size in a way that next read will give the number
                     // of entities
                     mesh.KwLocations[i] = (std::uint64_t)mesh.file.tellg() - line.size() + std::strlen(KeywordNames[i]);


                     break;
                }
            }
        }
    }
    // clear failbit, allow for more searches
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

    // Go to the position of kwd
    mesh.file.seekg( mesh.KwLocations[kwd] );
    
    std::istringstream stream = detail::getline(mesh.file);

    // read number of entities
    int64_t value;
    stream >> value;
    
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
void get_line(SU2_mesh& mesh, SU2Keyword type, double& x, double& y, double& z)
{
    assert(type == SU2Keyword::POINT3D && " keyword should be POINT3D");
    
    std::istringstream stream = detail::getline(mesh.file);

    stream >> x >> y >> z; 
}
// 2D points
void get_line(SU2_mesh& mesh, SU2Keyword type, double& x, double& y)
{
    assert(type == SU2Keyword::POINT2D &&  " keyword should be POINT2D");

    std::istringstream stream = detail::getline(mesh.file);

    stream >> x >> y; 
}

SU2Keyword get_element_type(SU2_mesh& mesh)
{
    int c;
    mesh.file >> c;
    SU2Keyword type = static_cast<SU2Keyword>(c);
    return type;
}

};
