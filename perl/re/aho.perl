

createNewPath
#
# input : ( $oldpath $newtop )
# return : $newpath
#
sub createNewPath
{
	$oldpath = shift; # /home/ringo/fuck$B$N$h$&$J(B
	$newtop = shift; # /home1/ringo/inside$B$N$h$&$J(B

# /home1/ringo/inside/fuck $B$rJV$9$N$,L\E*$G$"$k!#(B

	if( ( $oldpath =~ /\.\./)  || ( $oldpath =~ /\./ ) ){ return ""; }
	if( ( $newtop =~ /\.\./)  || ( $newtop =~ /\./ ) ){ return ""; }
	


$hoge = "../fuck/godemn.txt";


$hoge =~ s/^\.\//hoge/;





print "hoge:" , $hoge , "\n";
