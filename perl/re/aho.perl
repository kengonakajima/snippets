

createNewPath
#
# input : ( $oldpath $newtop )
# return : $newpath
#
sub createNewPath
{
	$oldpath = shift; # /home/ringo/fuckのような
	$newtop = shift; # /home1/ringo/insideのような

# /home1/ringo/inside/fuck を返すのが目的である。

	if( ( $oldpath =~ /\.\./)  || ( $oldpath =~ /\./ ) ){ return ""; }
	if( ( $newtop =~ /\.\./)  || ( $newtop =~ /\./ ) ){ return ""; }
	


$hoge = "../fuck/godemn.txt";


$hoge =~ s/^\.\//hoge/;





print "hoge:" , $hoge , "\n";
