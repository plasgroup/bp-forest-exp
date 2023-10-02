#!/bin/bash
mkdir -p build
echo "zipfian const, num of tasklets, total_num_requests, time_sendrequests[s], time_dpu_execution[s], total_time[s], throughput[OPS/s]" | tee ./data/ycsb.csv
NR_TASKLETS=24
NR_DPUS=2500
for i in 0 0.4 0.6 0.99 1.2
do
for m in 1 5 10 20 50 100
do
    echo "$i, $m"
    ./bp-forest/build/host/host_app -a $i -m $m -n 20000000| tee ./data/migration/${i}_${m}.csv
done
done
# do
# dpu-upmem-dpurte-clang -g -O3 -Wall -Werror -Wextra -flto=thin -Idpu/inc -Icommon/inc -DNR_TASKLETS=$i -DSTACK_SIZE_DEFAULT=256 dpu/src/bplustree.c dpu/src/dpumain.c -o build/dpu_task
# cc -o build/host_app host/src/pim_host.c `dpu-pkg-config --libs dpu` -fopenmp -g -Wall -Werror -Wextra -O3 -std=c99 `dpu-pkg-config --cflags dpu` -Ihost/inc -Icommon/inc -DNR_TASKLETS=$i -DWRITE_CSV -DDPU_BINARY=\"./build/dpu_task\"
# build/host_app > log/dpu/eval_taskletnum/${i}_upmem_pointerdram.txt
# done