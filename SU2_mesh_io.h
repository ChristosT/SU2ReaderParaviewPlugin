#ifndef SU2_MESH_IO_H
#define SU2_MESH_IO_H

#include <fstream>
#include <functional>
namespace SU2_MESH_IO
{

enum class file_mode {READ,WRITE};
enum SU2Keyword
{
    NDIME,
    NELEM,
    NPOIN,
    NMARK,
    POINT,
    NumKw,
    LINE = 3,
    TRIANGLE = 5,
    QUADRILATERAL = 9,
    TETRAHEDRON = 10,
    HEXAHEDRON = 12,
    PRISM = 13,
    PYRAMID = 14
};

struct SU2_mesh
{
    int dim; // Mesh dimension
    file_mode fmode; //
    std::fstream file;
    int64_t KwLocations[NumKw] = {-1};
    //TODO boolean flag wheter points have index or not

};


bool open_mesh(const char* filename, enum file_mode fmode, SU2_mesh& mesh);
/**
 * Read number entities under kwd and set position to this kwd
 */
int64_t stat_kwd(SU2_mesh& mesh, int kwd);

// 3D points
void get_line(SU2_mesh& mesh, double& x, double& y, double& z, int64_t& index);
// 2D points
void get_line(SU2_mesh& mesh, double& x, double& y, int64_t& index);

// This is a template used to help us check that all arguments of the variadic 
// function are the same. It is evaluated recursively at compile time
// source https://codereview.stackexchange.com/a/51533
template <typename ... Types>
struct template_all; // UNDEFINED

// base case for recursion : zero argumets return true
template <>
struct template_all<> : std::true_type {};

// a false case plus anything returns false
template <typename ... Types>
struct template_all<std::false_type, Types...> : std::false_type {};

// recursive step : a true case plus anything returns the result of the predicate
// applied on the rest of the arguments
template <typename ... Types>
struct template_all<std::true_type, Types...> : template_all<Types...>::type {};

template<typename... Args>
void get_line(SU2_mesh& mesh, SU2Keyword& type, Args&... arguments)
{
    static_assert( template_all< typename std::is_same<Args,int64_t>::type... >::value,
                   "all arguments should be of type int64_t");

    unsigned int c;
    mesh.file >> c;
    type = static_cast<SU2Keyword>(c);
    
    std::reference_wrapper<int64_t> vals[] = {arguments...};
    for( auto& i : vals)
    {
        mesh.file >> i.get();
    }
}
};
#endif /* SU2_MESH_IO_H */
