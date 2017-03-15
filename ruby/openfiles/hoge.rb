files=[]
500.times do |i|
  files[i]=File.open("/etc/hosts","r")
end

sleep 10000