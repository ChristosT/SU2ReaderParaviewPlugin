#ifndef SU2_MESH_IO_H
#define SU2_MESH_IO_H

#include <fstream>
#include <map>
#include <vector>
#include <string>
namespace SU2_MESH_IO
{

enum SU2Keyword
{
    NDIME,
    NELEM,
    NPOIN,
    NMARK,
    NumKw,
    POINT2D,
    POINT3D,
    LINE = 3,
    TRIANGLE = 5,
    QUADRILATERAL = 9,
    TETRAHEDRON = 10,
    HEXAHEDRON = 12,
    PRISM = 13,
    PYRAMID = 14
};
enum class file_mode {READ,WRITE};

struct SU2_mesh
{
    // TODO use pimpl to hide implentation
    int dim;            // Mesh dimension
    file_mode fmode;    
    std::fstream file;
    int64_t KwLocations[NumKw] = {-1}; // file offset from the beginning of the file
                                       // where Kword blocks are located

    std::map<std::string,int64_t> markers; // file offeset for boundary markers 

    //TODO  merge markers and KwLocations

};


bool open_mesh(const char* filename, enum file_mode fmode, SU2_mesh& mesh);

/**
 * Read number entities under kwd and set position to this kwd
 */
int64_t stat_kwd(SU2_mesh& mesh, int kwd);
/**
 * Read number entities under kwd and set position to this kwd
 */
int64_t stat_kwd(SU2_mesh& mesh, const std::string& marker_name);

/** 
 * Accquire the names of the markers in the file,
 * use these names in stat_kwd to retrieve nubmer of element under marker
 * and set the file pointer to the first element
 */
std::vector<std::string> get_marker_tags(SU2_mesh& mesh);
/**
 * Read type of next element
 */
SU2Keyword get_element_type(SU2_mesh& mesh);

    
template<typename T, typename... Args>
void get_line(SU2_mesh& mesh, SU2Keyword type, T& arg, Args&... arguments);

/**
 * Read next 3D point
 */
template <>
void get_line(SU2_mesh& mesh, SU2Keyword type, double& x, double& y, double& z);

/**
 * Read next 2D point
 */
template <>
void get_line(SU2_mesh& mesh, SU2Keyword type, double& x, double& y);


/** 
 * Read next element connectivity
 */
//LINE
extern template
void get_line(SU2_mesh& mesh, SU2Keyword type, uint64_t& a, uint64_t& b);

// TRIANGLE
extern template
void get_line(SU2_mesh& mesh, SU2Keyword type, uint64_t& a, uint64_t& b, uint64_t& c);

// QUADRILATERAL/ TETRAHEDRON
extern template void get_line(SU2_mesh& mesh, SU2Keyword type, uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d);

// PYRAMID
extern template
void get_line(SU2_mesh& mesh, SU2Keyword type, uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d, uint64_t& e);

// PRISM 
extern template
void get_line(SU2_mesh& mesh, SU2Keyword type, uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d, uint64_t& e, uint64_t& f);

// HEXAHEDRON
extern template
void get_line(SU2_mesh& mesh, SU2Keyword type, uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d, uint64_t& e, uint64_t& f, uint64_t& g, uint64_t& h);
};
#endif /* SU2_MESH_IO_H */
