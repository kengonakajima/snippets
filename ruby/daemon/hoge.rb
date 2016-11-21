
print File.absolute_path(__FILE__), "\n"

if ARGV[0]=="daemon" then
  Process.daemon
end

f=File.open("/tmp/gogogo","w")
f.write( File.absolute_path(__FILE__) )
f.close()


100.times do  |i|
  sleep 1
  print i, "\n"
  system("echo #{i} >> /tmp/poo")
end
  
