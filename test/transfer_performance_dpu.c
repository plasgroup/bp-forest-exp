#include "mram.h"
#include <stdio.h>
#include <stdlib.h>
#define NR_ELEMS (1024 * 1024 / 8)

__mram uint8_t transfer_buffer[NR_ELEMS];
int main()
{
    int sum = 0;
    for (int i = 0; i < NR_ELEMS; i++) {
        sum += transfer_buffer[i];
    }
    printf("sum = %d\n", sum);
    return 0;
}
