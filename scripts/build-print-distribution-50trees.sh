case "$PWD" in
    */bp-forest)
        break
    ;;
    *)
        echo "run this script in bp-forest directory"
        exit
    ;;
esac

BUILD_DIR="build/print-distribution-50trees"

OPTION="-S . -B $BUILD_DIR"

FLAGS="-O3"
FLAGS="$FLAGS -UPRINT_DEBUG -DPRINT_DISTRIBUTION -DNR_SEATS_IN_DPU=64 -DNR_INITIAL_TREES_IN_DPU=50"
if [ $# = 3 ]
then
    FLAGS="$FLAGS -DNUM_INIT_REQS=$1 -DSPLIT_THREASHOLD=$2 -DDEFAULT_NR_BATCHES=$3"
elif [ $# = 0 ]
then
    FLAGS="$FLAGS -DNUM_INIT_REQS=60000000"
else
    echo "invalid arguments"
    exit
fi
FLAGS_HOST="-mcmodel=large $FLAGS"
FLAGS_HOST="$FLAGS_HOST -UMEASURE_XFER_BYTES "
FLAGS_HOST="$FLAGS_HOST -DRANK_ORIENTED_XFER"

FLAGS_DPU="$FLAGS"

cmake $OPTION -DCMAKE_CXX_FLAGS="$FLAGS_HOST" -DCMAKE_C_FLAGS="$FLAGS_DPU"
pushd $BUILD_DIR
make -j
popd
