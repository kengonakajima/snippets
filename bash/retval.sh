if sh exit1.sh; then
	echo TRUE
else
	echo FALSE  # こっちにくる。つまり失敗ならelseの方
fi



if sh exit0.sh; then
	echo TRUE # こっち
else
	echo FALSE
fi

                