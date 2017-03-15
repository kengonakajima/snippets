x=0
y=0
t=Time.now
while TRUE
  x+=rand()
  if x > 1000000 then
    y+=1
    x=0
    nt = Time.now
    STDERR.print "loop #{y} dt:#{nt-t}\n"
    t=nt
  end
end
