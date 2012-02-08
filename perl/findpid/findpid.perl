
$p = `ps auxw`;


@ll = split( /\n/ , $p );

$hoge = "";

foreach $l ( @ll ){
	if( $l =~ /saac__/ ){
		$hoge = $l;
		last;
	}
}

($user,$pid)=split(/\s+/ , $hoge );


print $pid;
