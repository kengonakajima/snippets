#hoge
#shit
#foo
#bar
#baz
#ass
#sync


a = File.open( "aho.rb","r" )

a.seek(2, IO::SEEK_SET)

print a.readline,"\n"

a.seek(9, IO::SEEK_SET)

print a.readline, "\n"

