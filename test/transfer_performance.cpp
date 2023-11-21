#include <cstdio>
#include <cstdlib>
extern "C" {
#include <dpu.h>
#include <dpu_log.h>
}
#include <sys/time.h>

#ifndef DPU_BINARY
#define DPU_BINARY "./build/transfer_performance_dpu"
#endif
#ifndef NR_DPUS
#define NR_DPUS (256)
#endif
#define NR_ELEMS (1024 * 1024 / 8)

struct timeval start, end;
float time_diff(struct timeval* start, struct timeval* end)
{
    float timediff = (end->tv_sec - start->tv_sec) + 1e-6 * (end->tv_usec - start->tv_usec);
    return timediff;
}

uint8_t transfer_buffer[NR_DPUS][NR_ELEMS];
int main(void)
{
    struct dpu_set_t set, dpu;
    int each_dpu;
    DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));

    for (int i = 0; i < NR_DPUS; i++) {
        for (int j = 0; j < NR_ELEMS; j++) {
            transfer_buffer[i][j] = 1;
        }
    }
    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &transfer_buffer[each_dpu]));
    }
    gettimeofday(&start, NULL);
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "transfer_buffer", 0, NR_ELEMS, DPU_XFER_DEFAULT));
    gettimeofday(&end, NULL);
    printf("%d,%05f,", NR_DPUS, time_diff(&start, &end));
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    // DPU_FOREACH(set, dpu, each_dpu)
    // {
    //     DPU_ASSERT(dpu_log_read(dpu, stdout));
    // }
    for (int i = 0; i < NR_DPUS; i++) {
        for (int j = 0; j < NR_ELEMS; j++) {
            transfer_buffer[i][j] = 2;
        }
    }
    gettimeofday(&start, NULL);
    DPU_ASSERT(dpu_broadcast_to(set, "transfer_buffer", 0, &transfer_buffer, NR_ELEMS, DPU_XFER_DEFAULT));
    gettimeofday(&end, NULL);
    printf("%05f\n", time_diff(&start, &end));
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    // DPU_FOREACH(set, dpu, each_dpu)
    // {
    //     DPU_ASSERT(dpu_log_read(dpu, stdout));
    // }

    DPU_ASSERT(dpu_free(set));

    return 0;
}
