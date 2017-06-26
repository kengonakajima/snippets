HOGE=(A B C )
FUGA=(XX YY)
echo ${HOGE}
for x in ${HOGE[@]}; do
  for y in ${FUGA[@]}; do
    echo poo ${x} ${y}
  done
done



