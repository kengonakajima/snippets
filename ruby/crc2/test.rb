File.open("/etc/services").read.split("\n")[0..1000].each do |line|
  cout = `./a.out "#{line}"`.to_i
  rubyout = `ruby ./crc2.rb "#{line}"`.to_i
  STDERR.print "."
  raise "fail. cout=#{cout} rubyout=#{rubyout}" if ( cout != rubyout )
end
STDERR.print "ok\n"

