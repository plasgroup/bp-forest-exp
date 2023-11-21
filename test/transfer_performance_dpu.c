#include "mram.h"
#include <stdio.h>
#include <stdlib.h>
#define TRANSFER_BYTES_PER_DPU (1024 * 1024)

__mram uint8_t transfer_buffer[TRANSFER_BYTES_PER_DPU];
__host uint8_t transfer_buffer_wram[1024];
int main()
{
    int sum = 0;
    for (int i = 0; i < TRANSFER_BYTES_PER_DPU; i++) {
        sum += transfer_buffer[i];
    }
    printf("sum = %d\n", sum);
    return 0;
}
