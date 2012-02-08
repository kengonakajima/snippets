$fuck = "hoge hoge hoge(aho)aho" ;

$_ = $fuck;

@aaa = split( /[() ]/ );

foreach $a ( @aaa ){print "[$a]" , "\n"; }
