#!bin/bash
# experiment

# increment this variable when doing new experiment 
expno="2"

cd bp-forest

# build
bash ../scripts/build-release.sh
bash ../scripts/build-print-distribution.sh

for op in get insert
do
for build_type in print-distribution release
do 
mkdir -p ../${expno}/result/
for zipf_const in 0 0.2 0.4 0.6 0.8 0.99
do
build/${build_type}/host/host_app_UPMEM -a ${zipf_const} -b ${build_type} -o ${op} | tee ../${expno}/result/${build_type}_${zipf_const}_${op}
done
done
done
