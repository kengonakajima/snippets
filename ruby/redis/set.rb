require "rubygems"
require "redis"
require "redis/list"
require "redis/hash_key"
require "json"

Redis.current = Redis.new(  :host=>"localhost", :port=>6379 )

redis = Redis.current

redis.del "settest"
redis.del "settest2"

redis.sadd "settest", "kome"
redis.sadd "settest", "tamago"
redis.sadd "settest", "kome"

print redis.scard( "settest" ), "\n"

redis.sadd "settest2", "banana"
redis.sadd "settest2", "ringo"
redis.sadd "settest2", "kome"

out = redis.sinter "settest", "settest2"
out.each do |x|	print("x:",x,"\n")end

print "--\n"

out = redis.sdiff "settest", "settest2"
out.each do |x|	print("x:",x,"\n")end

redis.lpush "settest", "xx"   " -> error raised

