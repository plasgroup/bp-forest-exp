
case "$PWD" in
    */bp-forest)
	break
	;;
    *)
	echo "run this script in bp-forest directory"
	;;
esac

BUILD_DIR="build/release"

OPTION="-S . -B $BUILD_DIR"

FLAGS="-O3"
FLAGS="$FLAGS -UPRINT_DEBUG"

for zipf_const in 0 0.2 0.4 0.6 0.8 0.99 1.2
do
FLAGS_HOST="-mcmodel=large -DHOST_MULTI_THREAD=1 $FLAGS"
FLAGS_HOST="$FLAGS_HOST -UMEASURE_XFER_BYTES"
FLAGS_HOST="$FLAGS_HOST -DRANK_ORIENTED_XFER"

FLAGS_DPU="$FLAGS"

cmake $OPTION -DCMAKE_CXX_FLAGS="$FLAGS_HOST" -DCMAKE_C_FLAGS="$FLAGS_DPU"
pushd $BUILD_DIR
make -j
popd
./build/release/host/host_app_UPMEM -a ${zipf_const} -b release
done
done
