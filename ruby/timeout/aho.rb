#!/usr/bin/ruby
Thread.new do
  sleep 1
  STDERR.print "XXX\n"
  exit 1
end

a = `find /etc`

print a
