#include "libc.h"

int main(int argc, char** argv) {
    return close(-983243) == 0 ? 0 : -12;
}
