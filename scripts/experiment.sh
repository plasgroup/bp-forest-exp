#!bin/bash
# experiment

# increment this variable when doing new experiment 
expno="1"

cd bp-forest
mkdir -p ../${expno}/result/
for build_type in release-flat
do 
for zipf_const in 0 0.2 0.4 0.6 0.8 0.99 1.2
do
build/${build_type}/host/host_app_UPMEM -a ${zipf_const} -b ${build_type}| tee ../${date}/result/${build_type}_${zipf_const}
done
done
