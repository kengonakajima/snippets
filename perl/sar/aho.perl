

$hoge = "/lifestorm/fuck/shit";

@out = split( '/', $hoge );

print $aho = @out , "\n";
print "$out[0]\n";
print "$out[1]\n";
print "$out[2]\n";
print "$out[3]\n";

# $out[@out-1] ="";

$del = pop( @out );
$aa = join( '/' , @out );
print $aa , "\n", $del ,"\n";

