Signal.trap( :INT ) do
  print "aho\n"
end
Signal.trap( :INT ) do
  print "hoge\n"
end

while true
  STDERR.print "poo\n"
  sleep 1
end
