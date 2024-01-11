case "$PWD" in
    */bp-forest-exp)
	break
	;;
    *)
	echo "run this script in bp-forest-exp directory"
	exit
	;;
esac
# increment this variable when doing new experiment 
expno="1"

# clone repository
bash ./scripts/clone.sh
cd ./bp-forest
git checkout mthesis

# build
bash ../scripts/build-release.sh
bash ../scripts/build-print-distribution.sh

# generate workload
bash ./scripts/workload_gen.sh 100000000

# experiment

for build_type in release print-distribution
do 
mkdir -p ../${expno}/result/
for zipf_const in 0 0.2 0.4 0.6 0.8 0.99 1.2
do
build/${build_type}/host/host_app_UPMEM -a ${zipf_const} -b ${build_type}| tee ../${expno}/result/${build_type}_${zipf_const}
done
done
