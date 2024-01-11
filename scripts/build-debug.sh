
case "$PWD" in
    */bp-forest)
	break
	;;
    *)
	echo "run this script in bp-forest directory"
	exit
	;;
esac

BUILD_DIR="build/debug"

OPTION="-S . -B $BUILD_DIR"

FLAGS="-g"
FLAGS="$FLAGS -DPRINT_DEBUG=1 -DDEBUG_ON=1"

FLAGS_HOST="-mcmodel=large -O0 $FLAGS"
FLAGS_HOST="$FLAGS_HOST -DMEASURE_XFER_BYTES"
FLAGS_HOST="$FLAGS_HOST -DRANK_ORIENTED_XFER"

FLAGS_DPU="$FLAGS -O3"

cmake $OPTION -DCMAKE_CXX_FLAGS="$FLAGS_HOST" -DCMAKE_C_FLAGS="$FLAGS_DPU"
pushd $BUILD_DIR
make -j
popd
