require "rubygems"
require "redis"
require "redis/list"
require "redis/hash_key"
require "json"

Redis.current = Redis.new(  :host=>"localhost", :port=>6379 )

redis = Redis.current

redis.del "commits"
redis.set "foo","bar"
STDERR.print redis.get "foo"

STDERR.print "list\n"
N=1000
N.times do |i|
  qqq = {
    "poo" => "askdjfhaksjdhfkasjdf#{i}asdfalsjdflaksjd#{i}",
    "zoo" => { "BOO" => [i,i,i,i,i,i,i] }
  }
  redis.rpush( "commits", qqq.to_json )
end

aho = redis.lrange( "commits", 0, 100 )
aho.each do |e|
  STDERR.print JSON.parse(e)["zoo"]["BOO"][0]
end




