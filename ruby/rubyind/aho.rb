
a = <<EOF
aaa
aaa

dddddddd
eeee
eeee
EOF

print a.index( "\n\n" )

print a[10..-1]
