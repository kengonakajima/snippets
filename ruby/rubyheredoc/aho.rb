a = <<EOF
a
#{ system( "uptime" ) }
b

#{ ENV.keys.each{|i| print ENV[i] , "\n" }} 
EOF



print a
