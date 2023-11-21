#!/bin/bash
cd $(dirname $0)/../bp-forest
alphas=(0.6)
batch_size=(10 100 1000 10000 100000 1000000 10000000)
echo cd $(pwd)
for i in "${!alphas[@]}"
do
    echo "zipfian_const, num_dpus_redundant, num_dpus_multiple, num_tasklets, num_CPU_Trees, num_DPU_Trees, num_queries, num_reqs_for_cpu, num_reqs_for_dpu, num_reqs_{cpu/(cpu+dpu)}[%], send_time, execution_time_cpu, execution_time_cpu_and_dpu, exec_time_{cpu/(cpu&dpu)}[%], send_and_execution_time, total_time, throughput" | tee ./data/redundant_num_bptree_in_cpu_alpha${alphas[$i]}.csv
    for j in "${!batch_size[@]}"
    do
        cmake . ./build -DNR_TASKLETS=24 -DNR_DPUS=2500 -DCMAKE_BUILD_TYPE=Release > /dev/null 2>&1
        cmake --build ./build > /dev/null 2>&1
        ./build/host/host_app -n ${j}0 -${alphas[$i]} | tee -a ../data/preliminary/batch_size.csv
    done
done