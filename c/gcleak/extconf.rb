require "mkmf"
$CFLAGS="-g -I. -Wall"

$objs = ["hoge.o"]
create_makefile("hoge")
