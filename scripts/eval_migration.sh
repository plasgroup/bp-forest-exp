#!/bin/bash
mkdir -p build
NR_TASKLETS=24
NR_DPUS=2500
for i in 0.4 1.2 0
do
for m in 1 5 20
do
    echo "$i, $m"
    ./bp-forest/build/host/host_app -d ./bp-forest -a $i -m $m -n 100000000| tee ./data/migration_2/${i}_${m}.csv
done
done
# do
# dpu-upmem-dpurte-clang -g -O3 -Wall -Werror -Wextra -flto=thin -Idpu/inc -Icommon/inc -DNR_TASKLETS=$i -DSTACK_SIZE_DEFAULT=256 dpu/src/bplustree.c dpu/src/dpumain.c -o build/dpu_task
# cc -o build/host_app host/src/pim_host.c `dpu-pkg-config --libs dpu` -fopenmp -g -Wall -Werror -Wextra -O3 -std=c99 `dpu-pkg-config --cflags dpu` -Ihost/inc -Icommon/inc -DNR_TASKLETS=$i -DWRITE_CSV -DDPU_BINARY=\"./build/dpu_task\"
# build/host_app > log/dpu/eval_taskletnum/${i}_upmem_pointerdram.txt
# done