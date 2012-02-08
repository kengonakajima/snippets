#
#
#
# 空白文字でくぎるから、空白をエスケープする必要がある。
# エスケープする文字(とくべつないみのやつ)は以下
# ' '
# '\n'

print escape( $fuck = "aho is fuck.,\nnext line is also fuck.\n" );

print descape( $fuck );



sub escape($)
{
	$_ = shift;

	s/ /\\ /g;
	s/\n/\\n/g;

	return $_;
}

sub descape($)
{
	$_ = shift;

	s/\\n/\n/g;
	s/\\ / /g;

	return $_;
}


