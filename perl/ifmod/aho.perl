#
# Create HTTP-Date
#
#

my $hoge = gmtime( time());


my ( $wk, $mon, $day, $time, $year ) = split( /[ ]+/ , $hoge );

my ( $hour , $min , $sec ) = split( /:/ , $time );
$hour -=1;
print "If-Modified-Since: $wk, $day $mon $year $hour:$min:$sec GMT\n";
