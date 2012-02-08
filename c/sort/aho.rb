a = [ "a", "b", "e", "c", "d"]

a.sort! { |x,y|
  print "x:#{x} y:#{y}\n"
  if(x < y)then
    1
  else
    -1
  end
}

print a.join(":")
