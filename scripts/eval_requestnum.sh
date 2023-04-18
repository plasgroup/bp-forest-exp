#!/bin/bash
mkdir -p build
dpu-upmem-dpurte-clang -g -O3 -Wall -Werror -Wextra -flto=thin -Idpu/inc -Icommon/inc -DNUM_REQUESTS=$1 -DNR_DPUS=1 -DNR_TASKLETS=1 -DSTACK_SIZE_DEFAULT=256 dpu/src/bplustree.c dpu/src/dpumain.c -o build/dpu_task
cc -o build/host_app host/src/pim_host.c `dpu-pkg-config --libs dpu` -fopenmp -g -Wall -Werror -Wextra -O3 -std=c99 `dpu-pkg-config --cflags dpu` -Ihost/inc -Icommon/inc -DNR_TASKLETS=1 -DNUM_REQUESTS=$1 -DNR_DPUS=1 -DDPU_BINARY=\"./build/dpu_task\"
build/host_app > log/dpu/eval_requestnum/test.txt