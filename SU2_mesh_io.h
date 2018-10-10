#ifndef SU2_MESH_IO_H
#define SU2_MESH_IO_H

#include <fstream>
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
    int dim;            // Mesh dimension
    file_mode fmode;    
    std::fstream file;
    int64_t KwLocations[NumKw] = {-1}; // file offset from the beginning of the file
                                       // where Kword blocks are located
    //TODO boolean flag wheter points have index or not

};


bool open_mesh(const char* filename, enum file_mode fmode, SU2_mesh& mesh);

/**
 * Read number entities under kwd and set position to this kwd
 */
int64_t stat_kwd(SU2_mesh& mesh, int kwd);

/**
 * Read type of next element
 */
SU2Keyword get_element_type(SU2_mesh& mesh);

/**
 * Read next 3D point
 */
void get_line(SU2_mesh& mesh, SU2Keyword type, double& x, double& y, double& z, int64_t& index);

/**
 * Read next 2D point
 */
void get_line(SU2_mesh& mesh, SU2Keyword type, double& x, double& y, int64_t& index);

/** 
 * Read next element connectivity
 */

// base case for compile-time recursion
inline void get_line(SU2_mesh& mesh, SU2Keyword type) {};

template<typename T, typename... Args>
void get_line(SU2_mesh& mesh, SU2Keyword type, T& arg, Args&... arguments)
{
    static_assert( std::is_integral<T>::value, 
                     "all arguments should be of integral type");

    //TODO assert type matches number of elements

    mesh.file >> arg;

    get_line(mesh,type,arguments...);
}

};

#endif /* SU2_MESH_IO_H */
