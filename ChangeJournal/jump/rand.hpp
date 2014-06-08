#include <cstdint>
#include <vector>

#define _CRT_RAND_S
#include <stdlib.h>

namespace jump {
    template<typename T = uint8_t>
    std::vector<T> getRandVector(int size) {
        std::vector<T> retval(size);
        std::vector<unsigned int> random_data((size*sizeof(T)) / sizeof(unsigned int)+1);
        for (size_t i = 0; i < random_data.size(); i += 1) {
            // rand_s(&random_data[i]);
        }
        memcpy(retval.data(), random_data.data(), retval.size() * sizeof(T));
        return retval;
    }
}