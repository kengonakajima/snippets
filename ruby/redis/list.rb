require "redis"

redis = Redis.new(:host=>"localhost")

p redis

10000.times do |i|
  STDERR.print "." if i%100==0
  100.times do |j|
    redis.rpush "way_#{i}", j
  end
end

