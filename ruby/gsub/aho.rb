
a = "���\"asd����fas\"�ꤦ�󤮤���"

b = "\"asd����fas\""
a.gsub!( /#{b}/ , "BMC" )

print a  , "\n"
