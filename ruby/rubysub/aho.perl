
print &hoge(1);

sub hoge
{
	$a = shift(@_);
	return $a + 1;

}	 
	
