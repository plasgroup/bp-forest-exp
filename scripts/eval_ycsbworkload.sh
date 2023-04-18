#!/bin/bash
mkdir -p build
echo "zipfian const, num of tasklets, total_num_requests, time_sendrequests[s], time_dpu_execution[s], total_time[s], throughput[OPS/s]" | tee ./data/ycsb.csv
NR_TASKLETS=10
NR_DPUS=2500
for i in 0 0.5 0.99 1.2 1.5
do
    dpu-upmem-dpurte-clang -g -O3 -Wall -Werror -Wextra -flto=thin -Idpu/inc -Icommon/inc -DNR_TASKLETS=$NR_TASKLETS -DSTACK_SIZE_DEFAULT=256 dpu/src/bplustree.c dpu/src/dpumain.c -DNR_DPUS=$NR_DPUS -o build/dpu_task
    cc -o build/host_app host/src/pim_host.c `dpu-pkg-config --libs dpu` -fopenmp -g -Wall -Werror -Wextra -O3 -std=c99 `dpu-pkg-config --cflags dpu` -Ihost/inc -Icommon/inc -DFILE_NAME=\"./workload/zipfianconst$i.csv\" -DNR_TASKLETS=$NR_TASKLETS -DNR_DPUS=$NR_DPUS -DDPU_BINARY=\"./build/dpu_task\"
    echo -n "$i, " | tee -a ./data/ycsb.csv
    build/host_app | tee -a ./data/ycsb.csv
done

# do
# dpu-upmem-dpurte-clang -g -O3 -Wall -Werror -Wextra -flto=thin -Idpu/inc -Icommon/inc -DNR_TASKLETS=$i -DSTACK_SIZE_DEFAULT=256 dpu/src/bplustree.c dpu/src/dpumain.c -o build/dpu_task
# cc -o build/host_app host/src/pim_host.c `dpu-pkg-config --libs dpu` -fopenmp -g -Wall -Werror -Wextra -O3 -std=c99 `dpu-pkg-config --cflags dpu` -Ihost/inc -Icommon/inc -DNR_TASKLETS=$i -DWRITE_CSV -DDPU_BINARY=\"./build/dpu_task\"
# build/host_app > log/dpu/eval_taskletnum/${i}_upmem_pointerdram.txt
# done