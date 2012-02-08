require "commander.rb"

t = ARGV.join( " " ) 
exit 0 if( t == "" ) 

o,e = Commander.new( `cat hosts`.split ).command( t )
STDOUT.print o
STDERR.print e
