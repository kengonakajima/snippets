
a  = "vce-0_2_3_ahoahoah.zip"
a  = "vce-0_2_3_ahoahoah.tar.gz"

a =~ /vce-([0-9]+)_([0-9]+)_([0-9]+)_.+(\.zip|\.tar\.gz)/


print $1 , "\n"
print $2 , "\n"
print $3 , "\n"
print $4 , "\n"
