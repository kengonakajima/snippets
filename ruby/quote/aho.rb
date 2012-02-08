foo = "(.*)"
v = "asdas(.*)¤¢asdfas"

q= Regexp.quote( foo )


print v.gsub( q , "¤Û¤²" )

