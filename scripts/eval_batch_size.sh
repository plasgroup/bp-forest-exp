#!/bin/bash
cd $(dirname $0)/../bp-forest
alphas=(0.6)
batch_size=(10 100 1000 10000 100000 1000000 10000000)
echo cd $(pwd)
for j in "${!batch_size[@]}"
do
    rm -r ./build_UPMEM
    mkdir -p ./build_UPMEM
    cmake -S . -B ./build_UPMEM -DNR_TASKLETS=10 -DNUM_REQUESTS_PER_BATCH=${batch_size[$j]} -DNR_DPUS=2048 -DNR_SEATS_IN_DPU=20 -DCMAKE_BUILD_TYPE=Release ./build_UPMEM
    cmake --build ./build_UPMEM
    ./build_UPMEM/host/host_app -a ${alphas[$i]} | tee -a ../data/preliminary/batch_size.csv
done
