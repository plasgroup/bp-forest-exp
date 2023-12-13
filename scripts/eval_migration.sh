#!/bin/bash
mkdir -p build
for i in 0 0.99
do
for m in 5
do
for ops in get insert
do
    echo "$i, $m"
    ./bp-forest/build_UPMEM/host/host_app -a $i -m $m -n 5000000 -d bp-forest -o $ops | tee ./data/migration/${i}_${m}_$ops.csv
done
done
done
# do
# dpu-upmem-dpurte-clang -g -O3 -Wall -Werror -Wextra -flto=thin -Idpu/inc -Icommon/inc -DNR_TASKLETS=$i -DSTACK_SIZE_DEFAULT=256 dpu/src/bplustree.c dpu/src/dpumain.c -o build/dpu_task
# cc -o build/host_app host/src/pim_host.c `dpu-pkg-config --libs dpu` -fopenmp -g -Wall -Werror -Wextra -O3 -std=c99 `dpu-pkg-config --cflags dpu` -Ihost/inc -Icommon/inc -DNR_TASKLETS=$i -DWRITE_CSV -DDPU_BINARY=\"./build/dpu_task\"
# build/host_app > log/dpu/eval_taskletnum/${i}_upmem_pointerdram.txt
# done