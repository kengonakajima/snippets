
$a = time();
@hoge = localtime( $a );

print "sec: $hoge[0]\n";
print "min: $hoge[1]\n";
print "hour: $hoge[2]\n";
print "day: $hoge[3]\n";
print "mon: $hoge[4]\n";
print "year: $hoge[5]\n";

# 3かげつ延長
$a = $a + 60*60*24*93;


@hoge = localtime( $a );
print "----------\n";
print "sec: $hoge[0]\n";
print "min: $hoge[1]\n";
print "hour: $hoge[2]\n";
print "day: $hoge[3]\n";
print "mon: $hoge[4]\n";
print "year: $hoge[5]\n";
