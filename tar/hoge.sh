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

touch -d 2018-01-01 1/c
touch -d 2018-01-01 1/2/cc


tar zcfv 1.1.tar.gz -N 2017-11-11 1
echo OUTPUT of 1.1.tar.gz
tar tvfz 1.1.tar.gz

