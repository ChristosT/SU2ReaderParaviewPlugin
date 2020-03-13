#include "SU2_mesh_io.h"
#include <string>
#include <cstring>
#include <iostream>
#include <functional>
#include <sstream>
#include <cassert>
#include <array>
#include <limits>
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

// indexed by SU2Keyword element types
static const std::array<int,SU2Keyword::NumTotal> ElementSize = { 0,2,3,2,0,3,0,0,0,4,4,0,8,6,5};

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

    static void disregard_rest_of_line(std::fstream& file)
    {
        std::string tmp;
        std::getline(file,tmp); // disregard rest of line,
        (void) tmp;
    }

    static const char space = ' ';
        
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
            // markers treatment
            const char* key = "MARKER_TAG";
            int res = std::strncmp(line.c_str(),key, std::min(line.size(),std::strlen(key)));
            if( res == 0)
            {
                // split in spaces
                std::istringstream stream(line);
                
                std::string tag,tmp;
                
                stream >> tmp; // MARKER_TAG;
                stream >> tag; // actual tag
                //DPRINT(tag);

                std::getline(mesh.file,tmp); //next line
                DPRINT(tmp);

                mesh.markers[tag] = (uint64_t)mesh.file.tellg() - tmp.size() + std::strlen("MARKER_ELEMS");



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
        mesh.fmode = fmode;
        mesh.file.open(filename,std::ios::out);
        
        if( not mesh.file.is_open())
        {
            std::cerr<< " Could not open " << filename << std::endl;
            return false;
        }
        mesh.file.setf(std::ios_base::scientific);
        mesh.file.precision(std::numeric_limits< double > ::max_digits10);
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

void SU2_mesh::generate_element_type_counters() 
{
    // initialize map
    std::array<SU2Keyword,7> types ={ LINE,  TRIANGLE,  QUADRILATERAL ,   TETRAHEDRON ,  HEXAHEDRON ,   PRISM ,  PYRAMID };
    for(SU2Keyword type : types)
        this->element_counters[type] = 0;

    int64_t num = stat_kwd(*this,NELEM); // go to begining of elements section
    for(int64_t i = 0 ; i < num ; i++)
    {
        this->element_counters.at(get_element_type(*this))++;
        detail::disregard_rest_of_line(this->file);
    }

    for(const auto& item : this->markers)
    {
        int64_t num = stat_kwd(*this,item.first); // go to begining of marker
        for(int64_t i = 0 ; i < num ; i++)
        { 
            this->element_counters.at(get_element_type(*this))++;
            detail::disregard_rest_of_line(this->file);
        }
    }
}






void close_mesh(SU2_mesh& mesh)
{
    mesh.file.close();
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

    // Go to the position of kwd
    mesh.file.seekg( mesh.KwLocations[kwd] );
    
    std::istringstream stream = detail::getline(mesh.file);

    // read number of entities
    int64_t value;
    stream >> value;

    return value;
}
int64_t stat_kwd(SU2_mesh& mesh, const std::string& marker_name)
{
    if( marker_name.empty())
    {
        std::cerr<< "empty marker name" <<std::endl;
        return -1;
    }
    else if ( mesh.markers.find(marker_name) == mesh.markers.end())
    {
        std::cerr<< "non-existent marker" <<std::endl;
        return -2;
    }
    // Go to the position of kwd
    mesh.file.seekg( mesh.markers[marker_name] );
    
    std::istringstream stream = detail::getline(mesh.file);

    // read number of entities
    int64_t value;
    stream >> value;

    return value;
}
bool set_kwd(SU2_mesh& mesh, int kwd, int64_t k)
{
    if ( kwd < 0 or kwd >=NumKw)
    {
        std::cerr<< "Wrong kwd value";
        return false;
    }
    
    mesh.file << KeywordNames[kwd] << "=" << detail::space << k << std::endl;
    return true;
}
/**
 * Set number of entities under marker and set position to newline after
 */
bool set_kwd(SU2_mesh& mesh, const std::string& marker_name,int64_t k)
{
    mesh.file << "MARKER_TAG=" << detail::space << marker_name << std::endl;
    mesh.file << "MARKER_ELEMS=" << detail::space << k << std::endl;
    return true;
}



namespace detail
{
    // base case for compile-time recursion
    inline void get_line_impl(SU2_mesh& mesh) 
    {
        detail::disregard_rest_of_line(mesh.file);

    }

    template<typename T, typename... Args>
    void get_line_impl(SU2_mesh& mesh, T& arg, Args&... arguments)
    {
        static_assert( std::is_integral<T>::value, 
                       "all arguments should be of type uint64_t");


        mesh.file >> arg;

        get_line_impl(mesh,arguments...);
    }


}

template<typename T, typename... Args>
void get_line(SU2_mesh& mesh, SU2Keyword type, T& arg, Args&... arguments)
{
    static_assert( std::is_integral<T>::value, 
                     "all arguments should be of type uint64_t");

    if( sizeof...(arguments) + 1 != ElementSize.at(type))
    {
        std::cerr << "Wrong number of arguments" << std::endl;
        std::cerr << "Expected :" << ElementSize.at(type) << std::endl;
        std::cerr << "Got      :" << sizeof...(arguments) + 1  << std::endl;
        std::abort();
    }

    detail::get_line_impl(mesh,arg,arguments...);
}

// Explicit instatiation for connectivity reading functions
template
void get_line(SU2_mesh& mesh, SU2Keyword type, uint64_t& a, uint64_t& b);
// TRIANGLE
template
void get_line(SU2_mesh& mesh, SU2Keyword type, uint64_t& a, uint64_t& b, uint64_t& c);
// QUADRILATERAL/ TETRAHEDRON
template
void get_line(SU2_mesh& mesh, SU2Keyword type, uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d);
// PYRAMID
template
void get_line(SU2_mesh& mesh, SU2Keyword type, uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d, uint64_t& e);
// PRISM 
template
void get_line(SU2_mesh& mesh, SU2Keyword type, uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d, uint64_t& e, uint64_t& f);
// HEXAHEDRON
template
void get_line(SU2_mesh& mesh, SU2Keyword type, uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d, uint64_t& e, uint64_t& f, uint64_t& g, uint64_t& h);


// Specialization for reading point coordinates
template <>
void get_line(SU2_mesh& mesh, SU2Keyword type, double& x, double& y, double& z)
{
    assert(type == SU2Keyword::POINT3D && " keyword should be POINT3D");
    
    std::istringstream stream = detail::getline(mesh.file);


    stream >> x >> y >> z; 
}
// 2D points
template <>
void get_line(SU2_mesh& mesh, SU2Keyword type, double& x, double& y)
{
    assert(type == SU2Keyword::POINT2D &&  " keyword should be POINT2D");

    std::istringstream stream = detail::getline(mesh.file);

    stream >> x >> y; 
}







// base case for compile-time recursion
inline void set_line_impl(SU2_mesh& mesh) 
{
    mesh.file << "\n";

}


template<typename T, typename... Args>
void set_line_impl(SU2_mesh& mesh,T arg, Args... arguments)
{
    static_assert( std::is_integral<T>::value, 
                     "all arguments should be of type uint64_t");


    mesh.file << arg << detail::space ;

    set_line_impl(mesh,arguments...);
}

template<typename T, typename... Args>
void set_line(SU2_mesh& mesh, SU2Keyword type, T arg, Args... arguments)
{
    static_assert( std::is_integral<T>::value, 
                     "all arguments should be of type uint64_t");

    if( sizeof...(arguments) + 1 != ElementSize.at(type))
    {
        std::cerr << "Wrong number of arguments" << std::endl;
        std::cerr << "Expected :" << ElementSize.at(type) << std::endl;
        std::cerr << "Got      :" << sizeof...(arguments) + 1  << std::endl;
        std::abort();
    }

    mesh.file << type << detail::space ;

    set_line_impl(mesh,arg,arguments...);
}

// Explicit instatiation for connectivity reading functions
template
void set_line(SU2_mesh& mesh, SU2Keyword type, uint64_t a, uint64_t b);
// TRIANGLE
template
void set_line(SU2_mesh& mesh, SU2Keyword type, uint64_t a, uint64_t b, uint64_t c);
// QUADRILATERAL/ TETRAHEDRON
template
void set_line(SU2_mesh& mesh, SU2Keyword type, uint64_t a, uint64_t b, uint64_t c, uint64_t d);
// PYRAMID
template
void set_line(SU2_mesh& mesh, SU2Keyword type, uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e);
// PRISM 
template
void set_line(SU2_mesh& mesh, SU2Keyword type, uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f);
// HEXAHEDRON
template
void set_line(SU2_mesh& mesh, SU2Keyword type, uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g, uint64_t h);


// Specialization for reading point coordinates
template <>
void set_line(SU2_mesh& mesh, SU2Keyword type, double x, double y, double z)
{
    assert(type == SU2Keyword::POINT3D && " keyword should be POINT3D");
    
    mesh.file.precision(std::numeric_limits< double > ::max_digits10);
    mesh.file << x << detail::space << y << detail::space << z << std::endl;
}
// 2D points
template <>
void set_line(SU2_mesh& mesh, SU2Keyword type, double x, double y)
{
    assert(type == SU2Keyword::POINT2D &&  " keyword should be POINT2D");

    mesh.file.precision(std::numeric_limits< double > ::max_digits10);
    mesh.file << x << detail::space << y << detail::space << std::endl;
}

SU2Keyword get_element_type(SU2_mesh& mesh)
{
    int c;
    mesh.file >> c;
    SU2Keyword type = static_cast<SU2Keyword>(c);
    return type;
}
void set_element_type(SU2_mesh& mesh,SU2Keyword kwd)
{
    mesh.file << KeywordNames[kwd] << detail::space;
}
std::vector<std::string> get_marker_tags(SU2_mesh& mesh)
{
    std::vector<std::string> res;
    res.reserve(mesh.markers.size());
    for(auto& item : mesh.markers) res.push_back(item.first);
    return res;
}

}
