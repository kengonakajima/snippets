$src = "AHO+asfd?asdf%asdf";

$src =~ s/%/%BB/g;
$src =~ s/\+/%AA/g;
$src =~ s/\?/%CC/g;
print $src;


