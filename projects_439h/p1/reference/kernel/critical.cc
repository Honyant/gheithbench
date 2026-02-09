#include "critical.h"

Atomic<bool> c_flag { false };
volatile uint32_t c_depth = 0;
volatile uint32_t c_owner = uint32_t(-1);
