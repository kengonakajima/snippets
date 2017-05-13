rm -rf 1 *gz


mkdir 1
echo AAAAA > 1/a
echo BBBBB > 1/b
echo CCCCC > 1/c
mkdir 1/2
echo aaa > 1/2/aa
echo bbb > 1/2/bb
echo ccc > 1/2/cc

tar zcfv 1.tgz 1
wc 1.tgz

echo yyyy > 1/a
echo xxxx > 1/2/bb

tar zcfv 1.1.tar.gz -N diff 1
echo OUTPUT of 1.1.tar.gz
tar tvfz 1.1.tar.gz

