i = File.open( "aho.rb" , "r" )

h = Hash.new
h[ i ] = "asdf"

print h[i]
