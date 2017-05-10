PLATFORM=$1
shift
case "${PLATFORM}" in
mac | ios)
	OUTPUT=$1
	shift
	echo -create "$@" -output ${OUTPUT}
esac
