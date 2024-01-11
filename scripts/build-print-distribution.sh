case "$PWD" in
    */bp-forest)
	break
	;;
    *)
	echo "run this script in bp-forest directory"
	exit
	;;
esac

BUILD_DIR="build/print-distribution"

OPTION="-S . -B $BUILD_DIR"

FLAGS="-O3"
FLAGS="$FLAGS -UPRINT_DEBUG=1 -DPRINT_DISTRIBUTION"

FLAGS_HOST="-mcmodel=large $FLAGS"
FLAGS_HOST="$FLAGS_HOST -UMEASURE_XFER_BYTES "
FLAGS_HOST="$FLAGS_HOST -DRANK_ORIENTED_XFER"

FLAGS_DPU="$FLAGS"

cmake $OPTION -DCMAKE_CXX_FLAGS="$FLAGS_HOST" -DCMAKE_C_FLAGS="$FLAGS_DPU"
pushd $BUILD_DIR
make -j
popd
