f=File.open("hoge_rb.txt","w")
1000000.times do |i|
  f.printf("hoge:%d\n",i)
  f.flush()
end


  
