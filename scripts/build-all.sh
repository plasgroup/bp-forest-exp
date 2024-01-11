case "$PWD" in
    */bp-forest)
	break
	;;
    *)
	echo "run this script in bp-forest directory"
    exit
	;;
esac

for build_type in release release-flat xfer-flat xfer-rank debug print-distribution
do 
bash ../scripts/build-${build_type}.sh
done