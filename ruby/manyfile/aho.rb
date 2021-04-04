#Dir.mkdir("/tmp/ttt")
256.times do |x|
#  Dir.mkdir( "/tmp/ttt/#{x}")
  256.times do |y|
#    Dir.mkdir( "/tmp/ttt/#{x}/#{y}")
    32.times do |z|
      File.open("/tmp/ttt/#{x}/#{y}/#{z}.txt","a+") do |f|
        4.times do f.write("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n") end
      end
      STDERR.print "hoge:",x,",",y,",",z,"\n"
    end
  end  
end
