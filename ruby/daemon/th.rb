a=Thread.new  do
  while true
    sleep 1
    system("echo #{Time.now} #{Process.pid} >> /tmp/tttt")
  end
end

Process.daemon

10.times do 
  sleep 1
  system("echo #{Time.now} main #{Process.pid} >> /tmp/tttt")
end

