#!/bin/bash
mkdir -p build
for i in 2519
do
dpu-upmem-dpurte-clang -g -O3 -Wall -Werror -Wextra -flto=thin -Idpu/inc -Icommon/inc -DNUM_REQUESTS=$((i*200000)) -DNR_DPUS=$i -DNR_TASKLETS=1 -DSTACK_SIZE_DEFAULT=256 dpu/src/bplustree.c dpu/src/dpumain.c -o build/dpu_task
cc -o build/host_app host/src/pim_host.c `dpu-pkg-config --libs dpu` -fopenmp -g -Wall -Werror -Wextra -O3 -std=c99 `dpu-pkg-config --cflags dpu` -Ihost/inc -Icommon/inc -DNR_TASKLETS=1 -DNUM_REQUESTS=$((i*200000)) -DNR_DPUS=$i -DDPU_BINARY=\"./build/dpu_task\"
build/host_app > log/dpu/eval_dpunum/${i}_upmem.txt
done