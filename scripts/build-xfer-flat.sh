
case "$PWD" in
    */bp-forest)
	break
	;;
    *)
	echo "run this script in bp-forest directory"
	exit
	;;
esac

BUILD_DIR="build/xfer-flat"

OPTION="-S . -B $BUILD_DIR"

FLAGS="-O3"
FLAGS="$FLAGS -UPRINT_DEBUG"

FLAGS_HOST="-mcmodel=large $FLAGS"
FLAGS_HOST="$FLAGS_HOST -DMEASURE_XFER_BYTES"
FLAGS_HOST="$FLAGS_HOST -URANK_ORIENTED_XFER"

FLAGS_DPU="$FLAGS"

cmake $OPTION -DCMAKE_CXX_FLAGS="$FLAGS_HOST" -DCMAKE_C_FLAGS="$FLAGS_DPU"
pushd $BUILD_DIR
make -j
popd
