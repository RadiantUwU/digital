#pragma once
#include <stdlib.h>
#include <time.h> 

#define BOOL_RAND (rand() % 2)
#define BYTE_RAND (rand() % 256)
#define SHORT_RAND ((BYTE_RAND << 8) | BYTE_RAND)
#define LONG_RAND ((SHORT_RAND << 16) | SHORT_RAND)
#define LONGLONG_RAND ((LONG_RAND << 32) | LONG_RAND)

#define INIT_RAND() srand(time(NULL))