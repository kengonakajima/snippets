n=0
while 1
  l=gets
  if l =~ /^(\s+)<node/ then
    n+=1
    STDERR.print "n:#{n}\n" if(n%1000000==0)
  end
end
