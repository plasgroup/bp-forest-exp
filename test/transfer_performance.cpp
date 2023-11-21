#include <cstdio>
#include <cstdlib>
extern "C" {
#include <dpu.h>
#include <dpu_log.h>
}
#include <sys/time.h>
#include <memory>

#ifndef DPU_BINARY
#define DPU_BINARY "./build/transfer_performance_dpu"
#endif
#ifndef TRANSFER_BYTES_PER_DPU
#define TRANSFER_BYTES_PER_DPU (1024 * 16)
#endif
#ifndef NR_DPUS
#define NR_DPUS (1)
#endif
#define TRANSFER_BYTES_TOTAL (NR_DPUS * (uint64_t)TRANSFER_BYTES_PER_DPU)

struct timeval start, end;
float time_diff(struct timeval* start, struct timeval* end)
{
    float timediff = (end->tv_sec - start->tv_sec) + 1e-6 * (end->tv_usec - start->tv_usec);
    return timediff;
}


int main(void)
{
    uint8_t* transfer_buffer = (uint8_t*)malloc(TRANSFER_BYTES_TOTAL);
    struct dpu_set_t set, dpu;
    int each_dpu;
    DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
    for (uint64_t i = 0; i < TRANSFER_BYTES_TOTAL; i++) {
        transfer_buffer[i] = 1;
    }

// transfer to MRAM
// push_transfer
    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &transfer_buffer[each_dpu * TRANSFER_BYTES_PER_DPU]));
    }
    gettimeofday(&start, NULL);
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "transfer_buffer", 0, TRANSFER_BYTES_PER_DPU, DPU_XFER_DEFAULT));
    gettimeofday(&end, NULL);
    printf("%d,MRAM,%d,%5f,", TRANSFER_BYTES_PER_DPU, NR_DPUS, TRANSFER_BYTES_TOTAL / time_diff(&start, &end) / 1048576);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

// push_transfer(1 DPU in 4)
if (NR_DPUS >= 4) {
    DPU_FOREACH(set, dpu, each_dpu)
    {
        if (each_dpu / 4 == 0)
        DPU_ASSERT(dpu_prepare_xfer(dpu, &transfer_buffer[each_dpu * TRANSFER_BYTES_PER_DPU]));
    }
    gettimeofday(&start, NULL);
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "transfer_buffer", 0, TRANSFER_BYTES_PER_DPU, DPU_XFER_DEFAULT));
    gettimeofday(&end, NULL);
    printf("%5f,", TRANSFER_BYTES_TOTAL / time_diff(&start, &end) / 1048576 / 4);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
} else printf("0.00000,");

// push_transfer (serial)
    gettimeofday(&start, NULL);
    for (int i = 0; i < NR_DPUS; i++) {
        
        DPU_FOREACH(set, dpu, each_dpu)
        {
            if (each_dpu == i)
            DPU_ASSERT(dpu_prepare_xfer(dpu, &transfer_buffer[each_dpu * TRANSFER_BYTES_PER_DPU]));
        }

        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "transfer_buffer", 0, TRANSFER_BYTES_PER_DPU, DPU_XFER_DEFAULT));
    }
    gettimeofday(&end, NULL);
    printf("%5f,", TRANSFER_BYTES_TOTAL / time_diff(&start, &end) / 1048576);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
// broadcast
if (TRANSFER_BYTES_PER_DPU < 32*1024) {

    gettimeofday(&start, NULL);
    DPU_ASSERT(dpu_broadcast_to(set, "transfer_buffer", 0, &transfer_buffer, TRANSFER_BYTES_PER_DPU, DPU_XFER_DEFAULT));
    gettimeofday(&end, NULL);
    printf("%5f\n", TRANSFER_BYTES_TOTAL / time_diff(&start, &end) / 1048576);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
} else {
    printf("0.00000\n");
}

// transfer to WRAM
    if (TRANSFER_BYTES_PER_DPU < 32*1024) {
    // push_transfer
        DPU_FOREACH(set, dpu, each_dpu)
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &transfer_buffer[each_dpu * TRANSFER_BYTES_PER_DPU]));
        }
        gettimeofday(&start, NULL);
        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "transfer_buffer_wram", 0, TRANSFER_BYTES_PER_DPU, DPU_XFER_DEFAULT));
        gettimeofday(&end, NULL);
        printf("%d,WRAM,%d,%5f,", TRANSFER_BYTES_PER_DPU, NR_DPUS, TRANSFER_BYTES_TOTAL / time_diff(&start, &end) / 1048576);
        DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

    // push_transfer(1 DPU in 4)
    if (NR_DPUS >= 4) {
        DPU_FOREACH(set, dpu, each_dpu)
        {
            if (each_dpu / 4 == 0)
            DPU_ASSERT(dpu_prepare_xfer(dpu, &transfer_buffer[each_dpu * TRANSFER_BYTES_PER_DPU]));
        }
        gettimeofday(&start, NULL);
        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "transfer_buffer_wram", 0, TRANSFER_BYTES_PER_DPU, DPU_XFER_DEFAULT));
        gettimeofday(&end, NULL);
        printf("%5f,", TRANSFER_BYTES_TOTAL / time_diff(&start, &end) / 1048576 / 4);
        DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    } else printf("0.00000,");

    // push_transfer (serial)
        gettimeofday(&start, NULL);
        for (int i = 0; i < NR_DPUS; i++) {
            
            DPU_FOREACH(set, dpu, each_dpu)
            {
                if (each_dpu == i)
                DPU_ASSERT(dpu_prepare_xfer(dpu, &transfer_buffer[each_dpu * TRANSFER_BYTES_PER_DPU]));
            }

            DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "transfer_buffer_wram", 0, TRANSFER_BYTES_PER_DPU, DPU_XFER_DEFAULT));
        }
        gettimeofday(&end, NULL);
        printf("%5f,", TRANSFER_BYTES_TOTAL / time_diff(&start, &end) / 1048576);
        DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

    // broadcast
    if (TRANSFER_BYTES_PER_DPU < 32*1024) {

        gettimeofday(&start, NULL);
        DPU_ASSERT(dpu_broadcast_to(set, "transfer_buffer_wram", 0, &transfer_buffer, TRANSFER_BYTES_PER_DPU, DPU_XFER_DEFAULT));
        gettimeofday(&end, NULL);
        printf("%5f\n", TRANSFER_BYTES_TOTAL / time_diff(&start, &end) / 1048576);
        DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    } else {
        printf("0.00000\n");
    }
    }


    DPU_ASSERT(dpu_free(set));
    free(transfer_buffer);

    return 0;
}
