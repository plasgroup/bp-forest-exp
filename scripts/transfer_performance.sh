#!bin/bash

mkdir -p build
#echo NR_DPUS,push_transfer,push_transfer_1DPUin4,push_transfer_serial,broadcast | tee ./data/preliminary/transfer_performance.csv
for i in 1 2 4 8 16 32 64 128 256 512 1024 2048
do
dpu-upmem-dpurte-clang -g -O3 -Wall -Werror -Wextra -flto=thin -DNR_DPUS=$i -DNR_TASKLETS=1 -DSTACK_SIZE_DEFAULT=256 test/transfer_performance_dpu.c -o build/transfer_performance_dpu
cc test/transfer_performance.cpp `dpu-pkg-config --libs dpu` -fopenmp -g -Wall -Werror -Wextra -O3 -std=c++14 `dpu-pkg-config --cflags dpu` -DNR_DPUS=$i -DDPU_BINARY=\"./build/transfer_performance_dpu\" -o build/transfer_performance
build/transfer_performance | tee -a ./data/preliminary/transfer_performance.csv
done