require "redis"

redis = Redis.new(:host=>"localhost")

p redis

500000.times do |i|
  redis.set "node_#{i}",rand()
end

