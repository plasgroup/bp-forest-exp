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
#define TRANSFER_BYTES_PER_DPU (1024 * 16)

struct timeval start, end;
float time_diff(struct timeval* start, struct timeval* end)
{
    float timediff = (end->tv_sec - start->tv_sec) + 1e-6 * (end->tv_usec - start->tv_usec);
    return timediff;
}


int main(void)
{
    uint8_t* transfer_buffer = (uint8_t*)malloc(sizeof(uint8_t) * NR_DPUS * TRANSFER_BYTES_PER_DPU);
    struct dpu_set_t set, dpu;
    int each_dpu;
    DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));

// push_transfer
    memset(transfer_buffer, 1, TRANSFER_BYTES_PER_DPU*NR_DPUS);

    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &transfer_buffer[each_dpu * TRANSFER_BYTES_PER_DPU]));
    }
    gettimeofday(&start, NULL);
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "transfer_buffer", 0, TRANSFER_BYTES_PER_DPU, DPU_XFER_DEFAULT));
    gettimeofday(&end, NULL);
    printf("%d,%d,%05f,", TRANSFER_BYTES_PER_DPU, NR_DPUS, time_diff(&start, &end));
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

// push_transfer(1 DPU in 4)
    DPU_FOREACH(set, dpu, each_dpu)
    {
        if (each_dpu / 4 == 0)
        DPU_ASSERT(dpu_prepare_xfer(dpu, &transfer_buffer[each_dpu * TRANSFER_BYTES_PER_DPU]));
    }
    gettimeofday(&start, NULL);
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "transfer_buffer", 0, TRANSFER_BYTES_PER_DPU, DPU_XFER_DEFAULT));
    gettimeofday(&end, NULL);
    printf("%05f,", time_diff(&start, &end));
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

// push_transfer (serial)
    float time = 0;
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
    time += time_diff(&start, &end);
    printf("%05f,", time_diff(&start, &end));
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

// broadcast
    gettimeofday(&start, NULL);
    DPU_ASSERT(dpu_broadcast_to(set, "transfer_buffer", 0, &transfer_buffer, TRANSFER_BYTES_PER_DPU, DPU_XFER_DEFAULT));
    gettimeofday(&end, NULL);
    printf("%05f\n", time_diff(&start, &end));
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

    DPU_ASSERT(dpu_free(set));
    free(transfer_buffer);

    return 0;
}
