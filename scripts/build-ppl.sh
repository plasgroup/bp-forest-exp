
case "$PWD" in
    */bp-forest)
        break
    ;;
    *)
        echo "run this script in bp-forest directory"
        exit
    ;;
esac

BUILD_DIR="build/ppl"

OPTION="-S . -B $BUILD_DIR"

FLAGS="-O3"
FLAGS="$FLAGS -UPRINT_DEBUG"
if [ $# = 3 ]
then
    FLAGS="$FLAGS -DNUM_INIT_REQS=$1 -DSPLIT_THRESHOLD=$2 -DDEFAULT_NR_BATCHES=$3"
elif [ $# = 1 ]
then
    FLAGS="$FLAGS -DNUM_INIT_REQS=$1"
elif [ $# = 0 ]
then
    FLAGS="$FLAGS -DNUM_INIT_REQS=60000000"
else
    echo "invalid arguments"
    exit
fi
FLAGS_HOST="-mcmodel=large -DHOST_MULTI_THREAD=32 $FLAGS"
FLAGS_HOST="$FLAGS_HOST -UMEASURE_XFER_BYTES"
FLAGS_HOST="$FLAGS_HOST -DRANK_ORIENTED_XFER"
FLAGS_HOST="$FLAGS_HOST -DEXTRA_MIGRATION"

FLAGS_DPU="$FLAGS"

cmake $OPTION -DCMAKE_CXX_FLAGS="$FLAGS_HOST" -DCMAKE_C_FLAGS="$FLAGS_DPU"
pushd $BUILD_DIR
make -j
popd
