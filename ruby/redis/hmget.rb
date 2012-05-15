require "rubygems"
require "redis"
require "json"

Redis.current = Redis.new(  :host=>"localhost", :port=>6379 )

redis = Redis.current

redis.hset "testtest123", "aa", "aaaaaaaaaaa"
redis.hset "testtest123", "bb", "bbbbbbbbbbb"
redis.hset "testtest123", "cc", "ccccccccccc"

print redis.hget( "testtest123", "bb" ), "\n"

out=redis.hmget( "testtest123", "aa", "bb", "cc" )
out.each do |e| print e, "\n" end

ary = ["aa","bb","cc"]
out=redis.hmget( "testtest123", *ary )
out.each do |e| print e, "\n" end
