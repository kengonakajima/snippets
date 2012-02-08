exit if ARGV.size != 1
`cat #{ARGV.shift}`.split("<a href=\"").each { |x|
x =~ /(.*)\"><span.*$/
print $1,"\n" if($1)

}
