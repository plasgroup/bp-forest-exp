#!/bin/bash
mkdir -p build
NR_DPUS=2500
NR_TASKLETS=12
NUM_BPTREE_IN_CPU=(0 1 2 3 4 6 8 10 15 20 30 40 50)
FILE_NAME=(zipfianconst0.csv zipfianconst0.99.csv zipfianconst1.2.csv)
for j in ${FILE_NAME[@]}; do
    echo "num of tasklets,num of requests, time for sending task[s], time for execution[s], total time[s], throughput[OPS/s]" > ./data/${j}.csv
    for i in ${NUM_BPTREE_IN_CPU[@]}; do
        dpu-upmem-dpurte-clang -g -O3 -Wall -Werror -Wextra -flto=thin -Idpu/inc -Icommon/inc -DNR_DPUS=${NR_DPUS} -DNR_TASKLETS=${NR_TASKLETS} -DSTACK_SIZE_DEFAULT=256 dpu/src/bplustree.c dpu/src/dpumain.c -o build/dpu_task
        cc -o build/host_app host/src/pim_host.c other/bptree_CPU/bplustree.c `dpu-pkg-config --libs dpu` -fopenmp -g -Wall -Werror -Wextra -O3 -lpthread -std=c99 `dpu-pkg-config --cflags dpu` -Ihost/inc -Icommon/inc -DNR_DPUS=${NR_DPUS} -DNR_TASKLETS=${NR_TASKLETS}  -DNUM_BPTREE_IN_CPU=${i} -DDPU_BINARY=\"./build/dpu_task\"
        echo "Executing num_bptree_in_cpu = $i..."
            ./build/host_app | tee ./data/bplustree_fixed_placement_${j}.csv -a
    done
done