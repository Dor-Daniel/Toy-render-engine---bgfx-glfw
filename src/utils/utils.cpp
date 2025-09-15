#include <vector>
#include <stdexcept>

namespace Utils{
    template < typename T >
 
    void vector_to_array(const std::vector<T>& vec, T* out_arr, std::size_t out_count){
        if (!out_arr) throw std::invalid_argument("out_arr is null");
        if (vec.size() != out_count) throw std::runtime_error("size mismatch");
        std::copy(vec.begin(), vec.end(), out_arr);
    }
    
}