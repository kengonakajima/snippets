if ARGV[0]=="daemon" then
  Process.daemon
end

1000.times do  |i|
  sleep 1
  print i, "\n"
  system("echo #{i} >> /tmp/poo")
end
  
