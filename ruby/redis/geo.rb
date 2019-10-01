require "redis"

redis = Redis.new(host:"localhost")

STDERR.print "add\n"

10000.times do |i|
  redis.geoadd( "nodes2", [rand(), rand(), i] )
end
STDERR.print "radius\n"

100.times do |i|
  out=redis.georadius( ["nodes2",rand(),rand(),0.2,"km", :WITHCOORD ] )
  p out
end


