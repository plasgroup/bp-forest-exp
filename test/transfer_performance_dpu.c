#include "mram.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef TRANSFER_BYTES_PER_DPU
#define TRANSFER_BYTES_PER_DPU (1024 * 1024)
#endif

__mram uint8_t transfer_buffer[TRANSFER_BYTES_PER_DPU];
__host uint8_t transfer_buffer_wram[32*1024];
int main()
{
    int sum = 0;
    for (int i = 0; i < TRANSFER_BYTES_PER_DPU; i++) {
        sum += transfer_buffer[i];
    }
    printf("sum = %d\n", sum);
    for (int i = 0; i < 32*1024; i++) {
        sum += transfer_buffer_wram[i];
    }
    return 0;
}
