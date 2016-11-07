
require "pidfile"

if PidFile.running?
  print "running!\n"
  exit
end

print "not running, start!\n"

p = PidFile.new( :piddir => "/var/tmp", :pidfile => "hogehoge.pid" )

puts p.pidfile
puts p.piddir
puts p.pid
puts p.pidpath


10.times do |i|
  sleep 1
  print i, "\n"
end

