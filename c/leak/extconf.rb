require "mkmf"

$CFLAGS = "-I. -g -Wall"
$objs = [ "aho.o" ]

create_makefile( "aho" )
