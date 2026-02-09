#include "libc.h"

uint32_t string_to_int(const char* str) {
    uint32_t result = 0;
    uint32_t i = 0;

    // Convert each character to the corresponding integer value
    for (; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        } else {
            // Handle invalid characters
            return 0; // or some error code
        }
    }

    return result;
}

int main(int argc, char** argv) {
    //try to access kernel memory
    
    uint32_t memoryToProbe = string_to_int(argv[1]);
    ((uint32_t*) memoryToProbe)[0] = 666;
    return 999;
}
