if diff 1 2 ; then
	echo eq
else
	echo differ
fi

if diff 2 3 ; then
	echo eq
else
	echo differ
fi
