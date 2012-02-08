
@src = ( 100 .. 200);
@hoge = jEncode(893,@src);

print $hoge[0] , "\n";
print $hoge[1] , "\n";
print $hoge[2] , "\n";
print $hoge[99] , "\n";

@aaa = jDecode(893,@hoge);

print "--------\n";
print $aaa[0] , "\n";
print $aaa[1] , "\n";
print $aaa[99] , "\n";


#
# いっこのもじれつを1このcharの配列にする
#
sub makeCharArray($)
{
	my $src = shift;
	my $len = length($src);
	return unpack( "C${len}" , $src );
}

#
# いっこのcharの配列をいっこの文字列にする
#

sub makeStringFromArray(@)
{
	my $len = @_;
	return pack( "C${len}" , @_ );
}





#
# char の配列をもらってencodeして char の配列をかえす
#
sub jEncode($@)
{
	my $key = shift;
	my $len = @_;
	my @out;
	my $i;
	my $c_sum = 0 ;

	for($i=0;$i<$len;$i++){
		$sum = $sum + $_[$i];
		$sum = $sum & 255;

		print "SUm: $sum\n";

		if((($key%7)==($i%5))||(($key%2)==($i%2))){
			$_[$i] = (~$_[$i]) & 0xff;
		}
		print "src[" , $i , "]=" , $_[$i] , "\n";
	}

	for($i=0;$i<$len+1;$i++){
		if( abs(($key%$len)) > $i){
			$out[$i]=( $_[$i] + $sum*(($i*$i)%3) ) & 0xff;
		}
		if( abs(($key%$len))==$i){
			$out[$i] = $sum;
		}
		if( abs(($key%$len))<$i){
			$out[$i] = ( $_[$i-1] + $sum * (($i*$i)%7) ) & 0xff ;
		}
		print "out" , $i , ":" , $out[$i] , "\n";
	}
	return @out;
}

sub jDecode($@)
{
	my $key = shift;
	my $len = @_;
	my @out;
	my $sum = 0;
	
	$sum = $_[ abs($key%($len-1))];

	my $i;
	for($i=0;$i<$len;$i++){
		if( abs(($key%($len-1))) > $i ){
			$out[$i] = ($_[$i] - $sum*(($i*$i)%3))&0xff;
		}
		if( abs(($key%($len-1))) < $i ){
			$out[$i-1] = ( $_[$i] - $sum*(($i*$i)%7))&0xff;
		}
	}

	for($i=0;$i<($len-1);$i++){
		if( (($key %7) == ($i%5)) || ( $key %2) == ($i%2)){
			$out[$i] = (~ $out[$i] ) & 0xff ;
		}
	}
	return @out;

}
