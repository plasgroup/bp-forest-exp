#!bin/bash

expno="ppl_response_500M"

cd bp-forest

# build
bash ../scripts/build-release.sh 500000000
bash ../scripts/build-ppl.sh 500000000
bash ../scripts/build-release-50trees.sh 500000000
bash ../scripts/build-ppl-50trees.sh 500000000

mkdir -p ../${expno}/result/
for build_type in ppl ppl-50trees release release-50trees
do
    for zipf_const in 0 0.4 0.99
    do
        for op in get insert
        do
            build/${build_type}/host/host_app_UPMEM -a ${zipf_const} -b ${build_type} -o ${op} -m 5 | tee ../${expno}/result/${build_type}_${zipf_const}_${op}.csv
        done
    done
done

expno="ppl_response_60M_300batches"
bash ../scripts/build-release.sh 60000000 40000 300
bash ../scripts/build-print-distribution.sh 60000000 40000 300
bash ../scripts/build-ppl.sh 60000000 40000 300
bash ../scripts/build-release-50trees.sh 60000000 40000 300
bash ../scripts/build-print-distribution-50trees.sh 60000000 40000 300
bash ../scripts/build-ppl-50trees.sh 60000000 40000 300
mkdir -p ../${expno}/result/
for build_type in ppl ppl-50trees release release-50trees print-distribution print-distribution-50trees
do
    for zipf_const in 0 0.4 0.99
    do
        for op in get insert
        do
            build/${build_type}/host/host_app_UPMEM -a ${zipf_const} -b ${build_type} -o ${op} -m 5 | tee ../${expno}/result/${build_type}_${zipf_const}_${op}.csv
        done
    done
done

expno="ppl_response_memory"
# nr of init reqs, split threshold, nr of batches
bash ../scripts/build-print-distribution.sh 25000000 8300 500
bash ../scripts/build-print-distribution-50trees.sh 25000000 2000 500
mkdir -p ../${expno}/result/
for build_type in print-distribution
do
    for zipf_const in 0 0.4 0.99
    do
        for op in insert
        do
            build/${build_type}/host/host_app_UPMEM -a ${zipf_const} -b ${build_type} -o ${op} -m 5 | tee ../${expno}/result/${build_type}_${zipf_const}_${op}.csv
        done
    done
done
