case "$PWD" in
    */bp-forest-exp)
	break
	;;
    *)
	echo "run this script in bp-forest-exp directory"
	exit
	;;
esac
# experiment directory (change this variable to do new experiment) 
expno="reproduce"


# checkout
cd ./bp-forest
git checkout mthesis

# build
bash ../scripts/build-release.sh
bash ../scripts/build-print-distribution.sh

# generate workload
bash ./scripts/workload_gen.sh 600000000

# experiment
mkdir -p ../${expno}/result/
for operation in get insert
do
for zipf_const in 0 0.4 0.99
do
build/release/host/host_app_UPMEM -a ${zipf_const} -b release -o ${operation} | tee ../${expno}/result/release_${zipf_const}_${operation}
done
done

for operation in get insert
do
for zipf_const in 0 0.4 0.99
do
build/print-distribution/host/host_app_host_only -a ${zipf_const} -b print-distribution -o ${operation}| tee ../${expno}/result/print-distribution_${zipf_const}_${operation}
done
done
